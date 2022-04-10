#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <curses.h>
#include "mapBuilder.h"
#include "heatMap.h"
#include "minHeap.h"

/// Constructor/Destuctors

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

/// METHODS
cell* placeEntity(map *screen, minHeap *mh, char type)
{
    srand(time(NULL));

    int x, y, facing;
    bool in = false;

    if (type == '@') {
        int loc, dir, found = 0;

        while (found == 0) {

            dir = rand() % 2;

            if (dir == 0) {
                loc = (rand() % 78) + 2;

                for (int i = 3; i < 21; i++) {
                    if (screen->map[i][loc].type == '#' && screen->eMap[i][loc] == NULL) {
                        screen->eMap[i][loc] = (cell *) malloc(sizeof (cell));

                        screen->eMap[i][loc]->type = type;
                        screen->eMap[i][loc]->y = i;
                        screen->eMap[i][loc]->x = loc;
                        screen->eMap[i][loc]->weight = rand() % 4;

                        screen->eMap[i][loc]->dist = 10;

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
                loc = (rand() % 19) + 2;

                for (int i = 3; i < 80; i++) {
                    if (screen->map[loc][i].type == '#' && screen->eMap[loc][i] == NULL) {
                        screen->eMap[loc][i] = (cell *) malloc(sizeof (cell));

                        screen->eMap[loc][i]->type = type;
                        screen->eMap[loc][i]->y = loc;
                        screen->eMap[loc][i]->x = i;
                        screen->eMap[loc][i]->weight = rand() % 4;

                        screen->eMap[loc][i]->dist = 10;

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
        return screen->eMap[y][x];
    }

    while (!in) {
        x = (rand() % 78) + 1;
        y = (rand() % 19) + 1;
        facing = rand() % 4; // just a little random direction we can put into the cell for pacers and rand walkers

        if (screen->eMap[y][x] == NULL && screen->map[y][x].type != 'M' && screen->map[y][x].type != 'C' && screen->map[y][x].type != '%' && screen->map[y][x].type != '#') {
            // Malloc some space for our entity
            screen->eMap[y][x] = (cell *) malloc(sizeof (cell));

            // Set up our entity with its required values
            screen->eMap[y][x]->type = type;
            screen->eMap[y][x]->x = x;
            screen->eMap[y][x]->y = y;
            screen->eMap[y][x]->weight = facing; // Putting the facing in here since it is wasted space anyways right now
            screen->eMap[y][x]->instance = 0;
            screen->eMap[y][x]->inHeap = true;

            // Get the cost of the cell we were plopped down on for a good start time
            screen->eMap[y][x]->dist = determineCost(screen->map[y][x].type, type);

            // Add them to the heap
            mhAdd(mh, screen->eMap[y][x]);

            in = true;
        }
    }

    return screen->eMap[y][x];
}

void delEntity(map *screen, minHeap *mh, cell *entity)
{
    // Remove it from the heap
    mhDeleteElement(mh, entity);

    // Remove it from the map
    screen->eMap[entity->y][entity->x] = NULL;

    // free the memory
    free(entity);
}

int moveEntity(map *screen, minHeap *mh, cell *entity, cell *player)
{
    int iters, nX = entity->x, nY = entity->y;
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
                            fightPLayer(screen, entity, player);
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
                            fightPLayer(screen, entity, player);
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
                            fightPLayer(screen, entity, player);
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
                            fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                                fightPLayer(screen, entity, player);
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
                        fightPLayer(screen, entity, player);
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
                                    fightPLayer(screen, entity, player);
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
                                    fightPLayer(screen, entity, player);
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
                                    fightPLayer(screen, entity, player);
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
                                    fightPLayer(screen, entity, player);
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
    cell *tmp = screen->eMap[entity->y][entity->x];

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

void printScreen(map *screen, char str[])
{
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (screen->eMap[i][j] != NULL) {
                mvaddch(i + 1, j, screen->eMap[i][j]->type);
            } else {
                mvaddch(i + 1, j, screen->map[i][j].type);
            }
        }
    }

    mvprintw(0, 0, "You beat the %s!\n", str);

    refresh(); // actually displays the board
}

int fightPLayer(map *screen, cell *entity, cell *player)
{
    set_escdelay(10);

    printAmogus();

    mvprintw(18, 26, "Press Esc to win!");

    refresh();

    int ch;

    entity->inHeap = false;

    while (true) {
        ch = getch();

        if (ch == 27) {
            printScreen(screen, (char *) "enemy!");

            set_escdelay(1000);
            return 0;
        }
    }
}

void printAmogus() {
    for (int i = 4; i < 19; i++) {
        for (int j = 25; j < 55; j++) {
            mvaddch(i, j, ' ');
        }
    }

    for (int i = 34; i < 49; i++) {
        mvaddch(4, i, ACS_CKBOARD);
    }

    for (int i = 5; i < 16; i++) {
        mvaddch(i, 33, ACS_CKBOARD);
    }

    mvaddch(5, 49, ACS_CKBOARD);

    for (int i = 10; i < 16; i++) {
        mvaddch(i, 49, ACS_CKBOARD);
    }

    mvaddch(6, 50, ACS_CKBOARD);
    mvaddch(6, 49, ACS_CKBOARD);
    mvaddch(6, 48, ACS_CKBOARD);
    mvaddch(6, 47, ACS_CKBOARD);
    mvaddch(6, 46, ACS_CKBOARD);
    mvaddch(6, 45, ACS_CKBOARD);
    mvaddch(6, 44, ACS_CKBOARD);

    mvaddch(9, 50, ACS_CKBOARD);
    mvaddch(9, 49, ACS_CKBOARD);
    mvaddch(9, 48, ACS_CKBOARD);
    mvaddch(9, 47, ACS_CKBOARD);
    mvaddch(9, 46, ACS_CKBOARD);
    mvaddch(9, 45, ACS_CKBOARD);
    mvaddch(9, 44, ACS_CKBOARD);

    for (int i = 7; i < 9; i++) {
        mvaddch(i, 43, ACS_CKBOARD);
        mvaddch(i, 51, ACS_CKBOARD);
    }

    mvaddch(16, 34, ACS_CKBOARD);
    mvaddch(16, 35, ACS_CKBOARD);
    mvaddch(16, 36, ACS_CKBOARD);
    mvaddch(16, 37, ACS_CKBOARD);
    mvaddch(16, 38, ACS_CKBOARD);

    mvaddch(16, 48, ACS_CKBOARD);
    mvaddch(16, 47, ACS_CKBOARD);
    mvaddch(16, 46, ACS_CKBOARD);
    mvaddch(16, 45, ACS_CKBOARD);
    mvaddch(16, 44, ACS_CKBOARD);

    mvaddch(15, 39, ACS_CKBOARD);
    mvaddch(14, 39, ACS_CKBOARD);

    mvaddch(15, 43, ACS_CKBOARD);
    mvaddch(14, 43, ACS_CKBOARD);

    mvaddch(13, 42, ACS_CKBOARD);
    mvaddch(13, 41, ACS_CKBOARD);
    mvaddch(13, 40, ACS_CKBOARD);

    mvaddch(6, 32, ACS_CKBOARD);
    mvaddch(6, 31, ACS_CKBOARD);

    mvaddch(12, 32, ACS_CKBOARD);
    mvaddch(12, 31, ACS_CKBOARD);

    for (int i = 7; i < 12; i++) {
        mvaddch(i, 30, ACS_CKBOARD);
    }
}