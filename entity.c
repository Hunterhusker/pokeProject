#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "mapBuilder.h"
#include "heatMap.h"
#include "minHeap.h"

cell_t* placeEntity(map_t *screen, minHeap_t *mh, char type)
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

                for (int i = 1; i < 21; i++) {
                    if (screen->map[i][loc].type == '#' && screen->eMap[i][loc] == NULL) {
                        screen->eMap[i][loc] = malloc(sizeof (cell_t));

                        screen->eMap[i][loc]->type = type;
                        screen->eMap[i][loc]->y = i;
                        screen->eMap[i][loc]->x = loc;
                        screen->eMap[i][loc]->weight = rand() % 4;

                        screen->eMap[i][loc]->dist = determineCost(screen->map[i][loc].type, type);

                        // add this mf to the game time heap
                        mhAdd(mh, screen->eMap[i][loc]);

                        y = i;
                        x = loc;

                        found = 1;
                        break;
                    }
                }
            } else {
                loc = (rand() % 19) + 2;

                for (int i = 1; i < 80; i++) {
                    if (screen->map[loc][i].type == '#' && screen->eMap[loc][i] == NULL) {
                        screen->eMap[loc][i] = malloc(sizeof (cell_t));

                        screen->eMap[loc][i]->type = type;
                        screen->eMap[loc][i]->y = loc;
                        screen->eMap[loc][i]->x = i;
                        screen->eMap[loc][i]->weight = rand() % 4;

                        screen->eMap[loc][i]->dist = 0;

                        // add this mf to the game time heap
                        mhAdd(mh, screen->eMap[loc][i]);

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

        if (screen->eMap[y][x] == NULL && screen->map[y][x].type != 'M' && screen->map[y][x].type != 'C' && screen->map[y][x].type != '%') {
            // Malloc some space for our entity
            screen->eMap[y][x] = malloc(sizeof (cell_t));

            // Set up our entity with its required values
            screen->eMap[y][x]->type = type;
            screen->eMap[y][x]->x = x;
            screen->eMap[y][x]->y = y;
            screen->eMap[y][x]->weight = facing; // Putting the facing in here since it is wasted space anyways right now

            // Get the cost of the cell we were plopped down on for a good start time
            screen->eMap[y][x]->dist = determineCost(screen->map[y][x].type, type);

            // add this mf to the game time heap
            mhAdd(mh, screen->eMap[y][x]);

            in = true;
        }
    }

    return screen->eMap[y][x];
}

void delEntity(map_t *screen, minHeap_t *mh, cell_t *entity)
{
    // Remove it from the heap
    mhDeleteElement(mh, entity);

    // Remove it from the map
    screen->eMap[entity->y][entity->x] = NULL;

    // free the memory
    free(entity);
}

int moveEntity(map_t *screen, minHeap_t *mh, cell_t *entity)
{
    int mvCost;

    switch (entity->type) {
        bool valid = false;

        case '@':
            while (!valid) {
                switch (entity->weight) {
                    case 0: // North
                        mvCost = determineCost(screen->map[entity->y - 1][entity->x].type, entity->type);

                        if (screen->eMap[entity->y - 1][entity->x] == NULL || mvCost != 0 || mvCost != INT_MAX) {
                            valid = true;

                            screen->eMap[entity->y][entity->x]->dist += mvCost;

                            screen->eMap[entity->y - 1][entity->x] = screen->eMap[entity->y][entity->x]; // Move up
                            screen->eMap[entity->y][entity->x] = NULL; // Nullify the old spot
                        }
                        break;

                    case 1: // East
                        mvCost = determineCost(screen->map[entity->y][entity->x + 1].type, entity->type);

                        if (screen->eMap[entity->y][entity->x + 1] == NULL || mvCost != 0 || mvCost != INT_MAX) {
                            valid = true;

                            screen->eMap[entity->y][entity->x]->dist += mvCost;

                            screen->eMap[entity->y][entity->x + 1] = screen->eMap[entity->y][entity->x]; // Move over
                            screen->eMap[entity->y][entity->x] = NULL; // Nullify the old spot
                        }
                        break;

                    case 2: // South
                        mvCost = determineCost(screen->map[entity->y + 1][entity->x].type, entity->type);

                        if (screen->eMap[entity->y + 1][entity->x] == NULL || mvCost != 0 || mvCost != INT_MAX) {
                            valid = true;

                            screen->eMap[entity->y][entity->x]->dist += mvCost;

                            screen->eMap[entity->y + 1][entity->x] = screen->eMap[entity->y][entity->x]; // Move over
                            screen->eMap[entity->y][entity->x] = NULL; // Nullify the old spot
                        }
                        break;

                    case 3: // West
                        mvCost = determineCost(screen->map[entity->y][entity->x - 1].type, entity->type);

                        if (screen->eMap[entity->y][entity->x - 1] == NULL || mvCost != 0 || mvCost != INT_MAX) {
                            valid = true;

                            screen->eMap[entity->y][entity->x]->dist += mvCost;

                            screen->eMap[entity->y][entity->x - 1] = screen->eMap[entity->y][entity->x]; // Move down
                            screen->eMap[entity->y][entity->x] = NULL; // Nullify the old spot
                        }
                        break;

                    default: // *sobs uncontrollably*
                        return -1;
                }
            }

            entity->weight = rand() % 4;

            break;

        case 's': // don't move bro
            break;

        default:
            printf("ruh roh \n");
            return -1;
    }

    // Since we are only going to move the first thing in the heap, and we update its distance accordingly,
    //     we can just heapify down on the head of the heap to restore the heap property
    heapifyDown(mh, 0);

    return 0;
}