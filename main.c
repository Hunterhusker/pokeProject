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
    int entityCount;
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
    mvprintw(22, 0, "Use 10-key for movement; q to quit; check readme more controls");

    refresh(); // actually displays the board
}

void worldInit(gameBoard_t *world)
{
    gameBoardInit(world);
    
    world->board[world->currY][world->currX] = malloc(sizeof (map_t));
    
    generate(-1, -1, -1, -1, world->board[world->currY][world->currX], 100, world->entityCount);
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

    endwin(); // de-init the screen so we can end the program
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

    generate(exits[0], exits[1], exits[2], exits[3], world->board[y][x], buildingChance(x, y), world->entityCount);

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
        }

        printCurr(world, "simulation running");

        moveEntity(screen, mh, peek(mh), player);

        usleep(250000);
    }
}

void cursesInit()
{
    initscr();

    raw();
    noecho();
    curs_set(0);
}

void runGame(gameBoard_t *world, cell_t *player)
{
    char message[50] = "Your move!";
    int result;

    bool alive = true;
    while (alive) {
        while (peek(&world->board[world->currY][world->currX]->mh) != player) {
            moveEntity(world->board[world->currY][world->currX], &world->board[world->currY][world->currX]->mh, peek(&world->board[world->currY][world->currX]->mh), player);
        }

        printCurr(world, message);

        // reset the message
        strcpy(message, "Your move!");

        char ch = getch();

        if (ch == 'q') {
            alive = false;
        } else if (ch == '7' || ch == 'y') {
            result = movePlayer(player->y - 1, player->x - 1, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y - 1, player->x - 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == '8' || ch == 'k') {
            result = movePlayer(player->y - 1, player->x, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y - 1, player->x);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == '9' || ch == 'u') {
            result = movePlayer(player->y - 1, player->x + 1, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y - 1, player->x + 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == '4' || ch == 'h') {
            result = movePlayer(player->y, player->x - 1, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y, player->x - 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == '5' || ch == ' ') {
            sprintf(message, "Stood still...");
        } else if (ch == '6' || ch == 'l') {
            result = movePlayer(player->y, player->x + 1, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y, player->x + 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == '1' || ch == 'b') {
            result = movePlayer(player->y + 1, player->x - 1, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y + 1, player->x - 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == '2' || ch == 'j') {
            result = movePlayer(player->y + 1, player->x, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y + 1, player->x);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == '3' || ch == 'n') {
            result = movePlayer(player->y + 1, player->x + 1, world->board[world->currY][world->currX], player);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y + 1, player->x + 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 2) {
                sprintf(message, "GO TO NEXT SCREEN SOMEHOW");
            }
        } else if (ch == 'p') {
            sprintf(message, "player loc x: %d, y: %d", player->x, player->y);
        }
    }
}

int main(int argc, char *argv[])
{
    gameBoard_t world;
    cell_t *player; // a pointer to the player for better access
    int trainerCnt = 10;

    // Handle the switches
    if (argc >= 2) {
        if (!strcmp("--numtrainers", argv[1]) && argv[2] != NULL) {
            // Checks to make sure that the numbers after the switch are valid
            for (char *i = &argv[2][0]; *i != '\0'; i++) {
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

    // Set the world's trainer count, so we can better pass it around
    world.entityCount = trainerCnt;

    worldInit(&world);

    cursesInit();

    // Put the player into it's starting location
    player = placeEntity(world.board[world.currY][world.currX], &world.board[world.currY][world.currX]->mh,'@');

    runGame(&world, player);

    destroyWorld(&world); // must be run to collect garbage at the end

    return 0;
}
