#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <curses.h>
#include "mapBuilder.h"
#include "heatMap.h"
#include "minHeap.h"

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
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (world->board[world->currY][world->currX]->eMap[i][j] != NULL) {
                mvaddch(i, j, world->board[world->currY][world->currX]->eMap[i][j]->type);
            } else {
                mvaddch(i, j, world->board[world->currY][world->currX]->map[i][j].type);
            }
        }
    }

    mvprintw(21, 0, "Current Location: (%d, %d): %s\n", world->currX, world->currY, str);
    mvprintw(22, 0, "Check the readme for the control scheme");

    refresh();
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

//void userInput(gameBoard_t *world, cell_t *player, heatMap_t *heatMap)
//{
//    char in = ' ', enemyType;
//    char message[50] = "Welcome! Check the readme for commands";
//    int x = 0, y = 0, success;
//    bool printHM = false;
//
//    minHeap_t mh;
//
//    while (in != 'q')
//    {
//        //placePlayer(world->board[world->currY][world->currX], player);
////        placeEntity(world->board[world->currY][world->currX], &mh, '@');
////        placeEntity(world->board[world->currY][world->currX], &mh, 'h');
//
//        if (printHM) {
//            printHeatMap(heatMap);
//
//            printHM = false;
//        }
//
//        printCurr(world, message);
//
//        //unplacePlayer(world->board[world->currY][world->currX], player);
//
//        printf("> ");
//
//        scanf(" %c", &in);
//
//        if (in == 'n') {
//            // go north
//            success = goToLoc(world, world->currX, world->currY-1);
//
//            if (success != -1) {
//                strcpy(message, "Went north!");
//            } else {
//                strcpy(message, "Invalid, too far north!");
//            }
//        } else if (in == 's') {
//            // go south
//            success = goToLoc(world, world->currX, world->currY+1);
//
//            if (success != -1) {
//                strcpy(message, "Went south!");
//            } else {
//                strcpy(message, "Invalid, too far south!");
//            }
//        } else if (in == 'e') {
//            // go east
//            success = goToLoc(world, world->currX+1, world->currY);
//
//            if (success != -1) {
//                strcpy(message, "Went east!");
//            } else {
//                strcpy(message, "Invalid, too far east!");
//            }
//        } else if (in == 'w') {
//            // go west
//            success = goToLoc(world, world->currX-1, world->currY);
//
//            if (success != -1) {
//                strcpy(message, "Went west!");
//            } else {
//                strcpy(message, "Invalid, too far west!");
//            }
//        } else if (in == 'f') {
//            // fly away!
//            scanf(" %d %d", &x, &y);
//
//            success = goToLoc(world, x, y);
//
//            if (success != -1) {
//                sprintf(message, "Flew to (%d, %d)", x, y);
//            } else {
//                sprintf(message, "Can't fly to (%d, %d)", x, y);
//            }
//        } else if (in == 'h') {
//            scanf(" %c", &enemyType);
//
//            if (enemyType == 'H') {
//                fillHeatMap(world->board[world->currY][world->currX], heatMap, player, 'H');
//
//                printHM = true;
//
//                sprintf(message, "Printed the map for the hiker!");
//            } else if (enemyType == 'R') {
//                fillHeatMap(world->board[world->currY][world->currX], heatMap, player, 'R');
//
//                printHM = true;
//
//                sprintf(message, "Printed the map for the rival!");
//            } else {
//                sprintf(message, "Unknown enemy \"%c\", staying put!", enemyType);
//            }
//        } else if (in == 'q') {
//                printf("Goodbye!\n");
//        } else {
//            // Prints a custom error message (apparently sprintf is sketchy, so I will avoid it as much as I can)
//            sprintf(message, "Unknown input \"%c\", staying put!", in);
//        }
//    }
//}

void runSimulation(map_t *screen, minHeap_t *mh, gameBoard_t *world, cell_t *player)
{
    while (mh->currLen != 0) {

        while (peek(mh) != player) {
            moveEntity(screen, mh, peek(mh), player);

            //printCurr(world, "AHHHHH");
        }

        printCurr(world, "simulation running");

        moveEntity(screen, mh, peek(mh), player);

        usleep(250000);
    }
}

int placeEntities(int entityCount, map_t *screen, minHeap_t *mh)
{
    int type;

    for (int i = 0; i < entityCount; i++) {
        type = rand() % 6;

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

            case 5: // make a random walker
                placeEntity(screen, mh, 'r');
                break;

            default: // shit pants
                return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    initscr();

    gameBoard_t world;
    cell_t *player; // a pointer to the player for better access
    int trainerCnt = 10;

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

    player = placeEntity(world.board[world.currY][world.currX], &world.board[world.currY][world.currX]->mh,'@');

    placeEntities(trainerCnt, world.board[world.currY][world.currX], &world.board[world.currY][world.currX]->mh);

    // Runs the simulation
    //runSimulation(world.board[world.currY][world.currX], &world.board[world.currY][world.currX]->mh, &world, player);

    printCurr(&world, "poo");

    // This is where I would free the data, IF I COULD REACH IT!!!! DINKLEBERG
    destroyWorld(&world); // must be run to collect garbage at the end

    raw();
    noecho();
    curs_set(0);

    char ch = getch();

    if (ch == 'q') {
        endwin();
    }

    return 0;
}
