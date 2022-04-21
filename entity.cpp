#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <curses.h>
#include "mapBuilder.h"
#include "heatMap.h"
#include "minHeap.h"
#include "world.h"
#include "menus.h"

/// entity_cell stuff

/**
 * Deconstructor to ensure that we delete all of the trainer's pokemon when we end the game
 */
entity_cell::~entity_cell() {
    for (int i = 0; i < 6; i++) {
        if (this->pkmns[i] != NULL) {
            delete this->pkmns[i];
        }
    }
}

entity_cell::entity_cell(int x, int y, char type, int weight, int dist) {
    this->x = x;
    this->y = y;
    this->type = type;
    this->weight = weight;
    this->dist = dist;
    this->pkmnCnt = 0;

    for (int i = 0; i < 6; i++) {
        this->pkmns[i] = NULL;
    }
}

/// Player Cell constructor
player_cell::player_cell(int x, int y, char type, int weight, int dist, int ballCnt, int reviveCnt, int potionCnt) {
    this->x = x;
    this->y = y;
    this->type = type;
    this->weight = weight;
    this->dist = dist;
    this->pkmnCnt = 0;
    this->pokeballs = ballCnt;
    this->revives = reviveCnt;
    this->potions = potionCnt;

    for (int i = 0; i < 6; i++) {
        this->pkmns[i] = NULL;
    }
}

/// METHODS
player_cell *placePlayer(map *screen, minHeap *mh)
{
    srand(time(NULL));

    int x, y;

    int loc, dir, found = 0;

    while (found == 0) {

        dir = rand() % 2;

        if (dir == 0) {
            loc = (rand() % 76) + 2;

            for (int i = 3; i < 20; i++) {
                if (screen->map[i][loc].type == '#' && screen->eMap[i][loc] == NULL) {
                    screen->eMap[i][loc] = new player_cell(loc, i, '@', rand() % 4, 10, 5, 2, 3);

                    // add this mf to the game time heap
                    mhAdd(mh, screen->eMap[i][loc]);
                    screen->eMap[i][loc]->inHeap = true;

                    y = i;
                    x = loc;

                    found = 1;
                    break;
                }
            }
        } else {
            loc = (rand() % 17) + 2;

            for (int i = 3; i < 79; i++) {
                if (screen->map[loc][i].type == '#' && screen->eMap[loc][i] == NULL) {
                    screen->eMap[loc][i] = new player_cell(i, loc, '@', rand() % 4, 10, 5, 2, 3);

                    // add this mf to the game time heap
                    mhAdd(mh, screen->eMap[loc][i]);
                    screen->eMap[loc][i]->inHeap = true;

                    y = loc;
                    x = i;

                    found = 1;
                    break;
                }
            }
        }
    }

    // Return the pointer to the cell we made
    return (player_cell *) screen->eMap[y][x];
}

entity_cell* placeEntity(map *screen, minHeap *mh, char type)
{
    srand(time(NULL));

    int x, y, facing;
    bool in = false;

    while (!in) {
        x = (rand() % 78) + 1;
        y = (rand() % 19) + 1;
        facing = rand() % 4; // just a little random direction we can put into the cell for pacers and rand walkers

        if (screen->eMap[y][x] == NULL && screen->map[y][x].type != 'M' && screen->map[y][x].type != 'C' && screen->map[y][x].type != '%' && screen->map[y][x].type != '#') {
            // Malloc some space for our entity
            screen->eMap[y][x] = new entity_cell(x, y, type, facing, determineCost(screen->map[y][x].type, type)); //(cell *) malloc(sizeof (cell));

            screen->eMap[y][x]->instance = 0;
            screen->eMap[y][x]->inHeap = true;

            // Add them to the heap
            mhAdd(mh, screen->eMap[y][x]);

            in = true;
        }
    }

    return screen->eMap[y][x];
}

void delEntity(map *screen, minHeap *mh, entity_cell *entity)
{
    // Remove it from the heap
    mhDeleteElement(mh, entity);

    // Remove it from the map
    screen->eMap[entity->y][entity->x] = NULL;

    // free the memory
    free(entity);
}

int moveEntity(world *world, map *screen, minHeap *mh, entity_cell *entity, player_cell *player)
{
    int iters, nX = entity->x, nY = entity->y, fightResult;
    heatMap hm;
    bool valid;

    switch (entity->type) {
        case 'p': // Pacer movement code
            switch (entity->weight) {
                case 0: // north
                    // If the next space is occupied, or is a different terrain type, turn around and try to move
                    if (entity->y - 1 > 0 && (screen->eMap[entity->y - 1][entity->x] != NULL || determineCost(screen->map[entity->y - 1][entity->x].type, entity->type) == INT_MAX)) {
                        // Update the direction of the entity so it turns around
                        if (screen->eMap[entity->y - 1][entity->x] != NULL && screen->eMap[entity->y - 1][entity->x]->type == '@' && entity->inHeap == true) {
                            fightResult = fightPLayer(world, entity, player);

                            if (fightResult == -1) {
                                return -1;
                            }
                        }

                        entity->weight = 2;

                        // If the next space is valid, then we can go, if not, do nothing
                        if (entity->y + 1 < 20 && screen->eMap[entity->y + 1][entity->x] == NULL && determineCost(screen->map[entity->y + 1][entity->x].type, entity->type) != INT_MAX) {
                            nY++;
                        }
                    } else if (entity->y - 1 > 0 && screen->eMap[entity->y - 1][entity->x] == NULL && determineCost(screen->map[entity->y - 1][entity->x].type, entity->type) != INT_MAX) {
                        nY--;
                    } else {
                        entity->weight = 2;
                    }
                    break;

                case 1:
                    // If the next space is occupied, or is a different terrain type, turn around and try to move
                    if (entity->x + 1 < 79 && (screen->eMap[entity->y][entity->x + 1] != NULL || determineCost(screen->map[entity->y][entity->x + 1].type, entity->type) == INT_MAX)) {
                        // Update the direction of the entity so it turns around
                        if (screen->eMap[entity->y][entity->x + 1] != NULL && screen->eMap[entity->y][entity->x + 1]->type == '@' && entity->inHeap == true) {
                            fightResult = fightPLayer(world, entity, player);

                            if (fightResult == -1) {
                                return -1;
                            }
                        }

                        entity->weight = 3;

                        // If the next space is valid, then we can go, if not, do nothing
                        if (entity->x - 1 > 0 && screen->eMap[entity->y][entity->x - 1] == NULL && determineCost(screen->map[entity->y][entity->x - 1].type, entity->type) != INT_MAX) {
                            nX--;
                        }
                    } else if (entity->x + 1 < 79 && screen->eMap[entity->y][entity->x + 1] == NULL && determineCost(screen->map[entity->y][entity->x + 1].type, entity->type) != INT_MAX) {
                        nX++;
                    } else {
                        entity->weight = 3;
                    }
                    break;

                case 2:
                    // If the next space is occupied, or is a different terrain type, turn around and try to move
                    if (entity->y + 1 < 20 && (screen->eMap[entity->y + 1][entity->x] != NULL || determineCost(screen->map[entity->y + 1][entity->x].type, entity->type) == INT_MAX)) {
                        // Update the direction of the entity so it turns around
                        if (screen->eMap[entity->y + 1][entity->x] != NULL && screen->eMap[entity->y + 1][entity->x]->type == '@' && entity->inHeap == true) {
                            fightResult = fightPLayer(world, entity, player);

                            if (fightResult == -1) {
                                return -1;
                            }
                        }

                        entity->weight = 0;

                        // If the next space is valid, then we can go, if not, do nothing
                        if (entity->y - 1 > 0 && screen->eMap[entity->y - 1][entity->x] == NULL && determineCost(screen->map[entity->y - 1][entity->x].type, entity->type) != INT_MAX) {
                            nY--;
                        }
                    } else if (entity->y + 1 < 20 && screen->eMap[entity->y + 1][entity->x] == NULL && determineCost(screen->map[entity->y + 1][entity->x].type, entity->type) != INT_MAX) {
                        nY++;
                    } else {
                        entity->weight = 0;
                    }
                    break;

                case 3:
                    // If the next space is occupied, or is a different terrain type, turn around and try to move
                    if (entity->x - 1 > 0 && (screen->eMap[entity->y][entity->x - 1] != NULL || determineCost(screen->map[entity->y][entity->x - 1].type, entity->type) == INT_MAX)) {
                        // Update the direction of the entity so it turns around
                        if (screen->eMap[entity->y][entity->x - 1] != NULL && screen->eMap[entity->y][entity->x - 1]->type == '@' && entity->inHeap == true) {
                            fightResult = fightPLayer(world, entity, player);

                            if (fightResult == -1) {
                                return -1;
                            }
                        }

                        entity->weight = 1;

                        // If the next space is valid, then we can go, if not, do nothing
                        if (entity->x + 1 < 79 && screen->eMap[entity->y][entity->x + 1] == NULL && determineCost(screen->map[entity->y][entity->x + 1].type, entity->type) != INT_MAX) {
                            nX++;
                        }
                    } else if (entity->x - 1 > 0 && screen->eMap[entity->y][entity->x - 1] == NULL && determineCost(screen->map[entity->y][entity->x - 1].type, entity->type) != INT_MAX) {
                        nX--;
                    } else {
                        entity->weight = 1;
                    }
                    break;

                default:
                    return -1;
            }
            break;

        case 'w':
            valid = false;
            iters = 0;

            while (!valid) {
                switch (entity->weight) {
                    case 0: // north
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->y - 1 > 0 && (screen->eMap[entity->y - 1][entity->x] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y - 1][entity->x].type)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y - 1][entity->x] != NULL && screen->eMap[entity->y - 1][entity->x]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->y - 1 > 0 && screen->eMap[entity->y - 1][entity->x] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y - 1][entity->x].type) {
                            valid = true;
                            nY--;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;

                    case 1:
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->x + 1 < 79 && (screen->eMap[entity->y][entity->x + 1] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y][entity->x + 1].type)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y][entity->x + 1] != NULL && screen->eMap[entity->y][entity->x + 1]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->x + 1 < 79 && screen->eMap[entity->y][entity->x + 1] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y][entity->x + 1].type) {
                            valid = true;
                            nX++;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;

                    case 2:
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->y + 1 < 20 && (screen->eMap[entity->y + 1][entity->x] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y + 1][entity->x].type)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y + 1][entity->x] != NULL && screen->eMap[entity->y + 1][entity->x]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->y + 1 < 20 && screen->eMap[entity->y + 1][entity->x] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y + 1][entity->x].type) {
                            valid = true;
                            nY++;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;

                    case 3:
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->x - 1 > 0 && (screen->eMap[entity->y][entity->x - 1] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y][entity->x - 1].type)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y][entity->x - 1] != NULL && screen->eMap[entity->y][entity->x - 1]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->x - 1 > 0 && screen->eMap[entity->y][entity->x - 1] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y][entity->x - 1].type) {
                            valid = true;
                            nX--;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;
                }

                iters++;

                if (iters == 5) {
                    valid = true;
                    break;
                }
            }

            break;

        case 'n':
            valid = false;

            iters = 0;

            while (!valid) {
                switch (entity->weight) {
                    case 0: // north
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->y - 1 > 0 && (screen->eMap[entity->y - 1][entity->x] != NULL || determineCost(screen->map[entity->y - 1][entity->x].type, entity->type) == INT_MAX)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y - 1][entity->x] != NULL && screen->eMap[entity->y - 1][entity->x]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->y - 1 > 0 && screen->eMap[entity->y - 1][entity->x] == NULL && determineCost(screen->map[entity->y - 1][entity->x].type, entity->type) != INT_MAX) {
                            valid = true;
                            nY--;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;

                    case 1:
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->x + 1 < 79 && (screen->eMap[entity->y][entity->x + 1] != NULL || determineCost(screen->map[entity->y][entity->x + 1].type, entity->type) == INT_MAX)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y][entity->x + 1] != NULL && screen->eMap[entity->y][entity->x + 1]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->x + 1 < 79 && screen->eMap[entity->y][entity->x + 1] == NULL && determineCost(screen->map[entity->y][entity->x + 1].type, entity->type) != INT_MAX) {
                            valid = true;
                            nX++;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;

                    case 2:
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->y + 1 < 20 && (screen->eMap[entity->y + 1][entity->x] != NULL || determineCost(screen->map[entity->y + 1][entity->x].type, entity->type) == INT_MAX)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y + 1][entity->x] != NULL && screen->eMap[entity->y + 1][entity->x]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->y + 1 < 20 && screen->eMap[entity->y + 1][entity->x] == NULL && determineCost(screen->map[entity->y + 1][entity->x].type, entity->type) != INT_MAX) {
                            valid = true;
                            nY++;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;

                    case 3:
                        // If the next space is occupied, or is a different terrain type, turn around and try to move
                        if (entity->x - 1 > 0 && (screen->eMap[entity->y][entity->x - 1] != NULL || determineCost(screen->map[entity->y][entity->x - 1].type, entity->type) == INT_MAX)) {
                            // Update the direction of the entity, so it doesn't keep going straight
                            if (screen->eMap[entity->y][entity->x - 1] != NULL && screen->eMap[entity->y][entity->x - 1]->type == '@' && entity->inHeap == true) {
                                fightResult = fightPLayer(world, entity, player);

                                if (fightResult == -1) {
                                    return -1;
                                }
                            } else {
                                entity->weight = rand() % 4;
                            }
                        } else if (entity->x - 1 > 0 && screen->eMap[entity->y][entity->x - 1] == NULL && determineCost(screen->map[entity->y][entity->x - 1].type, entity->type) != INT_MAX) {
                            valid = true;
                            nX--;
                        } else {
                            entity->weight = rand() % 4;
                        }
                        break;
                }

                iters++;

                if (iters == 5) {
                    valid = true;
                    break;
                }
            }

            break;

        case 'h':
        case 'r':
            if (entity->inHeap == true) {
                // Fill up our heatmap w/ values for the hiker
                fillHeatMap(screen, &hm, player, entity->type);

                bool found = false;

                int x, y;

                minHeap neighbors;
                neighbors.currLen = 0;

                cell curr;

                // Add all neighbors to a local minHeap
                if (entity->y + 1 < 20) {
                    mhAdd(&neighbors, &hm.heatMap[entity->y + 1][entity->x]);

                    if (entity->x + 1 < 79) {
                        mhAdd(&neighbors, &hm.heatMap[entity->y + 1][entity->x + 1]);
                    }

                    if (entity->x - 1 > 0) {
                        mhAdd(&neighbors, &hm.heatMap[entity->y + 1][entity->x - 1]);
                    }
                }

                if (entity->y - 1 > 0) {
                    mhAdd(&neighbors, &hm.heatMap[entity->y - 1][entity->x]);

                    if (entity->x + 1 < 79) {
                        mhAdd(&neighbors, &hm.heatMap[entity->y - 1][entity->x + 1]);
                    }

                    if (entity->x - 1 > 0) {
                        mhAdd(&neighbors, &hm.heatMap[entity->y - 1][entity->x - 1]);
                    }
                }

                if (entity->x + 1 < 79) {
                    mhAdd(&neighbors, &hm.heatMap[entity->y][entity->x + 1]);
                }


                if (entity->x - 1 > 0) {
                    mhAdd(&neighbors, &hm.heatMap[entity->y][entity->x - 1]);
                }


                while (neighbors.currLen > 0 && !found) {
                    curr = mhExtract(&neighbors); // extract the location w/ smallest cost
                    x = curr.x;
                    y = curr.y;

                    // If there's space and the spot is traversable
                    if (screen->eMap[y][x] == NULL && determineCost(screen->map[y][x].type, entity->type) != INT_MAX) {
                        found = true; // Stop the loop, and go there

                        nX = x;
                        nY = y;

                        break;
                    } else if (screen->eMap[y][x]->type == '@') {
                        fightResult = fightPLayer(world, entity, player);

                        if (fightResult == -1) {
                            return -1;
                        }
                        found = true;
                    }
                }

                if (neighbors.currLen == 0) {
                    entity->dist += 40;
                }
            } else {
                // If they have been defeated, wander aimlessly in shame
                valid = false;
                iters = 0;

                if (screen->map[entity->y][entity->x].type == '#') {
                    if (entity->y - 1 > 0 && screen->map[entity->y - 1][entity->x].type != '#' && screen->eMap[entity->y - 1][entity->x] == NULL && determineCost(screen->map[entity->y - 1][entity->x].type, entity->type) != INT_MAX) {
                        nY--;
                        valid = true;
                    } else if (entity->x - 1 > 0 && screen->map[entity->y][entity->x - 1].type != '#' && screen->eMap[entity->y][entity->x - 1] == NULL && determineCost(screen->map[entity->y][entity->x - 1].type, entity->type) != INT_MAX) {
                        nX--;
                        valid = true;
                    } else if (entity->x + 1 < 78 && screen->map[entity->y][entity->x + 1].type != '#' && screen->eMap[entity->y][entity->x - 1] == NULL && determineCost(screen->map[entity->y][entity->x + 1].type, entity->type) != INT_MAX) {
                        nX++;
                        valid = true;
                    } else if (entity->y + 1 < 20 && screen->map[entity->y + 1][entity->x].type != '#' && screen->eMap[entity->y + 1][entity->x] == NULL && determineCost(screen->map[entity->y + 1][entity->x].type, entity->type) != INT_MAX) {
                        nY++;
                        valid = true;
                    }
                }

                while (!valid) {
                    switch (entity->weight) {
                        case 0: // north
                            // If the next space is occupied, or is a different terrain type, turn around and try to move
                            if (entity->y - 1 > 0 && (screen->eMap[entity->y - 1][entity->x] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y - 1][entity->x].type)) {
                                // Update the direction of the entity, so it doesn't keep going straight
                                if (screen->eMap[entity->y - 1][entity->x] != NULL && screen->eMap[entity->y - 1][entity->x]->type == '@' && entity->inHeap == true) {
                                    fightResult = fightPLayer(world, entity, player);

                                    if (fightResult == -1) {
                                        return -1;
                                    }
                                } else {
                                    entity->weight = rand() % 4;
                                }
                            } else if (entity->y - 1 > 0 && screen->eMap[entity->y - 1][entity->x] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y - 1][entity->x].type) {
                                valid = true;
                                nY--;
                            } else {
                                entity->weight = rand() % 4;
                            }
                            break;

                        case 1:
                            // If the next space is occupied, or is a different terrain type, turn around and try to move
                            if (entity->x + 1 < 79 && (screen->eMap[entity->y][entity->x + 1] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y][entity->x + 1].type)) {
                                // Update the direction of the entity, so it doesn't keep going straight
                                if (screen->eMap[entity->y][entity->x + 1] != NULL && screen->eMap[entity->y][entity->x + 1]->type == '@' && entity->inHeap == true) {
                                    fightResult = fightPLayer(world, entity, player);

                                    if (fightResult == -1) {
                                        return -1;
                                    }
                                } else {
                                    entity->weight = rand() % 4;
                                }
                            } else if (entity->x + 1 < 79 && screen->eMap[entity->y][entity->x + 1] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y][entity->x + 1].type) {
                                valid = true;
                                nX++;
                            } else {
                                entity->weight = rand() % 4;
                            }
                            break;

                        case 2:
                            // If the next space is occupied, or is a different terrain type, turn around and try to move
                            if (entity->y + 1 < 20 && (screen->eMap[entity->y + 1][entity->x] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y + 1][entity->x].type)) {
                                // Update the direction of the entity, so it doesn't keep going straight
                                if (screen->eMap[entity->y + 1][entity->x] != NULL && screen->eMap[entity->y + 1][entity->x]->type == '@' && entity->inHeap == true) {
                                    fightResult = fightPLayer(world, entity, player);

                                    if (fightResult == -1) {
                                        return -1;
                                    }
                                } else {
                                    entity->weight = rand() % 4;
                                }
                            } else if (entity->y + 1 < 20 && screen->eMap[entity->y + 1][entity->x] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y + 1][entity->x].type) {
                                valid = true;
                                nY++;
                            } else {
                                entity->weight = rand() % 4;
                            }
                            break;

                        case 3:
                            // If the next space is occupied, or is a different terrain type, turn around and try to move
                            if (entity->x - 1 > 0 && (screen->eMap[entity->y][entity->x - 1] != NULL || screen->map[entity->y][entity->x].type != screen->map[entity->y][entity->x - 1].type)) {
                                // Update the direction of the entity, so it doesn't keep going straight
                                if (screen->eMap[entity->y][entity->x - 1] != NULL && screen->eMap[entity->y][entity->x - 1]->type == '@' && entity->inHeap == true) {
                                    fightResult = fightPLayer(world, entity, player);

                                    if (fightResult == -1) {
                                        return -1;
                                    }
                                } else {
                                    entity->weight = rand() % 4;
                                }
                            } else if (entity->x - 1 > 0 && screen->eMap[entity->y][entity->x - 1] == NULL && screen->map[entity->y][entity->x].type == screen->map[entity->y][entity->x - 1].type) {
                                valid = true;
                                nX--;
                            } else {
                                entity->weight = rand() % 4;
                            }
                            break;
                    }

                    iters++;

                    if (iters == 5) {
                        valid = true;
                        break;
                    }
                }
            }

            break;

        case 's':
            // Add a check to see if the player is in the neighborhood
            break;

        default:
            return -1;
    }

    /// Actually move the entity now that we have found their new location

    // Grab the pointer to the entity
    entity_cell *tmp = screen->eMap[entity->y][entity->x];

    // Nullify the old place of the entity
    screen->eMap[entity->y][entity->x] = NULL;

    // Move the entity to the new place we found for them
    screen->eMap[nY][nX] = tmp;

    // Set the new game time for the entity
    entity->dist += determineCost(screen->map[nY][nX].type, entity->type);

    // Update the old location in the screen buffer to not have the entity
    mvaddch(entity->y + 1, entity->x, screen->map[entity->y][entity->x].type);

    // Move the entity to the new location in the screen buffer
    mvaddch(nY + 1, nX, entity->type);

    // Update the entity to know its new location
    entity->y = nY;
    entity->x = nX;

    // Since we are only going to move the first thing in the heap, and we update its distance accordingly,
    //     we can just heapify down on the head of the heap to restore the heap property
    heapifyDown(mh, 0);

    return 0;
}

//void printScreen(map *screen, char str[])
//{
//    for (int i = 0; i < 21; i++) {
//        for (int j = 0; j < 80; j++) {
//            if (screen->eMap[i][j] != NULL) {
//                mvaddch(i + 1, j, screen->eMap[i][j]->type);
//            } else {
//                mvaddch(i + 1, j, screen->map[i][j].type);
//            }
//        }
//    }
//
//    mvprintw(0, 0, "You beat the %s!\n", str);
//
//    refresh(); // actually displays the board
//}

int movePlayer(int y, int x, world *world, player_cell *player, std::string message) {
    // if the location is invalid, then return -1 to denote the error
    if (y > 20 || y < 0 || x > 79 || x < 0) {
        return -1;
    }

    // Check the map to see if the location is non-traversable, return 1 to denote a non-error non move
    if (world->board[world->currY][world->currX]->map[y][x].type == '%' || world->board[world->currY][world->currX]->eMap[y][x] != NULL) {
        if (world->board[world->currY][world->currX]->eMap[y][x] != NULL && world->board[world->currY][world->currX]->eMap[y][x]->inHeap == false) {
            return 3;
        } else if (world->board[world->currY][world->currX]->eMap[y][x] != NULL && world->board[world->currY][world->currX]->eMap[y][x]->inHeap == true) {
            int fightResult = fightPLayer(world, world->board[world->currY][world->currX]->eMap[y][x], player);

            if (fightResult == -1) {
                return -2;
            }

            return 0;
        }

        return 1;
    }

    // Check for the exit case, aka if on a border and the space is a #, return a 2 to say we hit an exit
    if ((y == 0 || y == 20 || x == 0 || x == 79) && world->board[world->currY][world->currX]->map[y][x].type == '#') {
        int nX, nY, pX, pY;

        if (y == 0) {
            nX = world->currX;
            nY = world->currY - 1;

            pX = x;
            pY = 19;


            // Add a message to let the player know what happened
            //sprintf(message, "Went North");
            message = "Went North";
        } else if (y == 20) {
            nX = world->currX;
            nY = world->currY + 1;

            pX = x;
            pY = 1;


            // Add a message to let the player know what happened
            //sprintf(message, "Went South");
            message = "Went South";
        } else if (x == 0) {
            nX = world->currX - 1;
            nY = world->currY;

            pX = 78;
            pY = y;


            // Add a message to let the player know what happened
            message = "Went West";
        } else if (x == 79) {
            nX = world->currX + 1;
            nY = world->currY;

            pX = 1;
            pY = y;


            // Add a message to let the player know what happened
            message = "Went East";
        }

        // A check to see if the space is occupied, if so, then we will stay put and tell the user their path is blocked
        if (world->board[nY][nX] != NULL) {
            if (world->board[nY][nX]->eMap[pY][pX] != NULL) {
                return 2;
            }
        }

        // Grab the player from the current screen
        entity_cell *temp = world->board[world->currY][world->currX]->eMap[player->y][player->x];

        // Nullify the old location of the player, since they're no longer there
        world->board[world->currY][world->currX]->eMap[player->y][player->x] = NULL;

        // Remove the player from the heap
        mhDeleteElement(&world->board[world->currY][world->currX]->mh, player);

        // Move boards
        goToLoc(world, nX, nY);

        // Add the player into the end of the new map
        world->board[world->currY][world->currX]->eMap[pY][pX] = temp;
        player->y = pY;
        player->x = pX;

        // Set the player's distance to be the first place in the heap TODO: might want this to be different
        player->dist = peek(&world->board[world->currY][world->currX]->mh)->dist - 1;

        // Add the player to the heap and buzz off
        mhAdd(&world->board[world->currY][world->currX]->mh, player);

        // Update the buffer
        printCurr(world);


        // return 0 since we were successful in moving
        return 0;
    }

    // If we have passed those tests, then we can probably just move there
    int cost = determineCost(world->board[world->currY][world->currX]->map[y][x].type, '@');
    player->dist += cost;

    world->board[world->currY][world->currX]->eMap[y][x] = world->board[world->currY][world->currX]->eMap[player->y][player->x];

    world->board[world->currY][world->currX]->eMap[player->y][player->x] = NULL;

    mvaddch(player->y + 1, player->x, world->board[world->currY][world->currX]->map[player->y][player->x].type);
    mvaddch(y + 1, x, '@');

    player->y = y;
    player->x = x;

    if (world->board[world->currY][world->currX]->map[y][x].type == ':') {
        int fight = rand() % 10;

        if (fight == 0) {
            fightRandomPokemon(abs(world->currX - 199) + abs(world->currY - 199), player);

            printCurr(world);
            //sprintf(message, "You beat the pokemon!");
            message = "You beat the pokemon!";
        }
    }

    // Since the player has moved and their gameTime has updated, then we need to send them further into the heap
    heapifyDown(&world->board[world->currY][world->currX]->mh, 0);

    mvprintw(0, 0, message.c_str());

    return 0;
}