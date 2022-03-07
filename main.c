#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mapBuilder.h"
#include "heatMap.h"

typedef struct gameBoard
{
    map_t *board[399][399];
    int currX;
    int currY;
} gameBoard_t;

void gameBoardInit(gameBoard_t *world)
{
    world->currX = 199;
    world->currY = 199;
    
    for (int i = 0; i < 399; i++)
    {
        for (int j = 0; j < 399; j++)
        {
            if (world->board[i][j] != NULL)
            {
                world->board[i][j] = NULL;
            }
        }
    }
}

void printCurr(gameBoard_t *world, char str[])
{
    printMap(world->board[world->currY][world->currX]);

    printf("Current Location: (%d, %d): %s\n", world->currX, world->currY, str);
    printf("Commands: n, s, e, w, f x y, h 'type', & q to quit: check ReadMe for more\n");
}

void worldInit(gameBoard_t *world)
{
    gameBoardInit(world);
    
    world->board[world->currY][world->currX] = malloc(sizeof (map_t));
    
    generate(-1, -1, -1, -1, world->board[world->currY][world->currX], 100);
}

void destroyWorld(gameBoard_t *world)
{
    for (int i = 0; i < 399; i++)
    {
        for (int j = 0; j < 399; j++)
        {
            if (world->board[i][j] != NULL)
            {
                deleteAllEntities(world->board[i][j]->eMap); // added deletion of all entities

                free(world->board[i][j]);
            }
        }
    }
}

int isScreenGenerated(gameBoard_t *world, int x, int y)
{
    if (world->board[y][x] != NULL) {
        return 1;
    }

    return 0;
}

int buildingChance(int x, int y)
{
    int dist = abs(x - 199) + abs(y - 199);

    int chance = ((dist * -45) / 200) + 50;

    if (chance <= 0) {
        return 0;
    }

    return chance;
}

int goToLoc(gameBoard_t *world, int x, int y)
{
    int exits[4] = {-1, -1, -1, -1};

    if (x >= 399 || x < 0 || y >= 399 || y < 0)
    {
        return -1;
    }

    if (isScreenGenerated(world, x, y) == 1) {
        world->currX = x;
        world->currY = y;

        return 0;
    }

    if (x + 1 < 399 && isScreenGenerated(world, x + 1, y) == 1) {
        exits[3] = world->board[y][x+1]->exits[2][0];
    } else if (x + 1 >= 399) {
        // set no exit cond
        exits[3] = 0;
    }

    if (x - 1 >= 0 && isScreenGenerated(world, x - 1, y) == 1) {
        exits[2] = world->board[y][x-1]->exits[3][0];
    } else if (x - 1 < 0) {
        // set no exit cond
        exits[2] = 0;
    }

    if (y + 1 < 399 && isScreenGenerated(world, x, y + 1) == 1) {
        exits[1] = world->board[y+1][x]->exits[0][1];
    } else if (y + 1 >= 399) {
        // set no exit cond
        exits[1] = 0;
    }

    if (y - 1 >= 0 && isScreenGenerated(world, x, y - 1) == 1) {
        exits[0] = world->board[y-1][x]->exits[1][1];
    } else if (y - 1 < 0) {
        // set no exit cond
        exits[0] = 0;
    }

    world->board[y][x] = malloc(sizeof (map_t));

    generate(exits[0], exits[1], exits[2], exits[3], world->board[y][x], buildingChance(x, y));

    world->currX = x;
    world->currY = y;

    return 0;
}

void userInput(gameBoard_t *world, cell_t *player, heatMap_t *heatMap)
{
    char in = ' ', enemyType;
    char message[50] = "Welcome! Check the readme for commands";
    int x = 0, y = 0, success;
    bool printHM = false;

    minHeap_t mh;

    while (in != 'q')
    {
        //placePlayer(world->board[world->currY][world->currX], player);
//        placeEntity(world->board[world->currY][world->currX], &mh, '@');
//        placeEntity(world->board[world->currY][world->currX], &mh, 'h');

        if (printHM) {
            printHeatMap(heatMap);

            printHM = false;
        }

        printCurr(world, message);

        //unplacePlayer(world->board[world->currY][world->currX], player);

        printf("> ");

        scanf(" %c", &in);

        if (in == 'n') {
            // go north
            success = goToLoc(world, world->currX, world->currY-1);

            if (success != -1) {
                strcpy(message, "Went north!");
            } else {
                strcpy(message, "Invalid, too far north!");
            }
        } else if (in == 's') {
            // go south
            success = goToLoc(world, world->currX, world->currY+1);

            if (success != -1) {
                strcpy(message, "Went south!");
            } else {
                strcpy(message, "Invalid, too far south!");
            }
        } else if (in == 'e') {
            // go east
            success = goToLoc(world, world->currX+1, world->currY);

            if (success != -1) {
                strcpy(message, "Went east!");
            } else {
                strcpy(message, "Invalid, too far east!");
            }
        } else if (in == 'w') {
            // go west
            success = goToLoc(world, world->currX-1, world->currY);

            if (success != -1) {
                strcpy(message, "Went west!");
            } else {
                strcpy(message, "Invalid, too far west!");
            }
        } else if (in == 'f') {
            // fly away!
            scanf(" %d %d", &x, &y);

            success = goToLoc(world, x, y);

            if (success != -1) {
                sprintf(message, "Flew to (%d, %d)", x, y);
            } else {
                sprintf(message, "Can't fly to (%d, %d)", x, y);
            }
        } else if (in == 'h') {
            scanf(" %c", &enemyType);

            if (enemyType == 'H') {
                fillHeatMap(world->board[world->currY][world->currX], heatMap, player, 'H');

                printHM = true;

                sprintf(message, "Printed the map for the hiker!");
            } else if (enemyType == 'R') {
                fillHeatMap(world->board[world->currY][world->currX], heatMap, player, 'R');

                printHM = true;

                sprintf(message, "Printed the map for the rival!");
            } else {
                sprintf(message, "Unknown enemy \"%c\", staying put!", enemyType);
            }
        } else if (in == 'q') {
                printf("Goodbye!\n");
        } else {
            // Prints a custom error message (apparently sprintf is sketchy, so I will avoid it as much as I can)
            sprintf(message, "Unknown input \"%c\", staying put!", in);
        }
    }
}

int placeEntities(int entityCount, map_t *screen, minHeap_t *mh)
{
    int type;

    for (int i = 0; i < entityCount; i++) {
        type = rand() % 5;

        switch (type) {
            case 0: // make a hiker
                placeEntity(screen, mh, 'h');
                break;

            case 1: // make a pacer
                placeEntity(screen, mh, 'p');
                break;

            case 2: // make a wanderer
                placeEntity(screen, mh, 'w');
                break;

            case 3: // make a stationary
                placeEntity(screen, mh, 's');
                break;

            case 4: // make a random walker
                placeEntity(screen, mh, 'n');
                break;

            default: // shit pants
                return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    gameBoard_t world;
    cell_t *player; // a pointer to the player for better access
    heatMap_t heatMap;
    int trainerCnt = 10;

    minHeap_t gameTime;
    gameTime.currLen = 0; // wasteful to add a whole init for the one line

    worldInit(&world);

    // Handle the switches
    if (argc >= 2) {
        if (!strcmp("--numtrainers", argv[1]) && argv[2] != NULL) {
            // Checks to make sure that the numbers after the switch are valid
            for (char *i = &argv[2][0]; *i != NULL; i++) {
                if (isdigit(*i) == 0) {
                    fprintf(stderr, "Usage: %s --numtrainers <int>\n", argv[0]);
                    return -1;
                }
            }

            trainerCnt = atoi(argv[2]);
        } else {
            // Print an error message and end
            fprintf(stderr, "Usage: %s --numtrainers <int>\n", argv[0]);

            return -1;
        }
    }

    // Add the player and save a pointer to them for to do stuff
    player = placeEntity(world.board[world.currY][world.currX], &gameTime, '@');

    placeEntities(trainerCnt, world.board[world.currY][world.currX], &gameTime);

    printCurr(&world, "oopsies");

    moveEntity(world.board[world.currY][world.currX], &gameTime, player);

    printCurr(&world, "oopsies");

    //userInput(&world, &player, &heatMap); // if we are running this as a sim, we def don't need user input lol

    destroyWorld(&world); // must be run to collect garbage at the end

    return 0;
}
