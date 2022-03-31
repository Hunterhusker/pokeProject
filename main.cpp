#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <curses.h>
#include "mapBuilder.h"
#include "heatMap.h"
#include "minHeap.h"
#include "fileReader.h"
#include "pokemon.h"
#include "pokemon_species.h"
#include "experience.h"
#include "type_names.h"
#include "pokemon_moves.h"
#include "moves.h"
#include <fstream>
#include <vector>

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
                mvaddch(i + 1, j, world->board[world->currY][world->currX]->eMap[i][j]->type);
            } else {
                mvaddch(i + 1, j, world->board[world->currY][world->currX]->map[i][j].type);
            }
        }
    }

    mvprintw(0, 0, "Current Location: (%d, %d): %s\n", world->currX, world->currY, str);
    //mvprintw(22, 0, "Use 10-key for movement; q to quit; check readme more controls");

    refresh(); // actually displays the board
}

void worldInit(gameBoard_t *world)
{
    gameBoardInit(world);
    
    world->board[world->currY][world->currX] = (map_t *) malloc(sizeof (map_t));
    
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

    world->board[y][x] = (map_t *) malloc(sizeof (map_t));

    generate(exits[0], exits[1], exits[2], exits[3], world->board[y][x], buildingChance(x, y), world->entityCount);

    world->currX = x;
    world->currY = y;

    return 0;
}

void cursesInit()
{
    initscr();

    raw();
    noecho();
    curs_set(0);

    keypad(stdscr, TRUE);
}

int movePlayer(int y, int x, gameBoard_t *world, cell_t *player, char message[]) {
    // if the location is invalid, then return -1 to denote the error
    if (y > 20 || y < 0 || x > 79 || x < 0) {
        return -1;
    }

    // Check the map to see if the location is non-traversable, return 1 to denote a non-error non move
    if (world->board[world->currY][world->currX]->map[y][x].type == '%' || world->board[world->currY][world->currX]->eMap[y][x] != NULL) {
        if (world->board[world->currY][world->currX]->eMap[y][x] != NULL && world->board[world->currY][world->currX]->eMap[y][x]->inHeap == false) {
            return 3;
        } else if (world->board[world->currY][world->currX]->eMap[y][x] != NULL && world->board[world->currY][world->currX]->eMap[y][x]->inHeap == true) {
            fightPLayer(world->board[world->currY][world->currX], world->board[world->currY][world->currX]->eMap[y][x], player);

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
            sprintf(message, "Went North");
        } else if (y == 20) {
            nX = world->currX;
            nY = world->currY + 1;

            pX = x;
            pY = 1;


            // Add a message to let the player know what happened
            sprintf(message, "Went South");
        } else if (x == 0) {
            nX = world->currX - 1;
            nY = world->currY;

            pX = 78;
            pY = y;


            // Add a message to let the player know what happened
            sprintf(message, "Went West");
        } else if (x == 79) {
            nX = world->currX + 1;
            nY = world->currY;

            pX = 1;
            pY = y;


            // Add a message to let the player know what happened
            sprintf(message, "Went East");
        }

        // A check to see if the space is occupied, if so, then we will stay put and tell the user their path is blocked
        if (world->board[nY][nX] != NULL) {
            if (world->board[nY][nX]->eMap[pY][pX] != NULL) {
                return 2;
            }
        }

        // Grab the player from the current screen
        cell_t *temp = world->board[world->currY][world->currX]->eMap[player->y][player->x];

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
        printCurr(world, message);

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

    // Since the player has moved and their gameTime has updated, then we need to send them further into the heap
    heapifyDown(&world->board[world->currY][world->currX]->mh, 0);

    return 0;
}

// Makes the black box so that our menus are all the same size
void menuInit() {
    for (int i = 2; i < 21; i++) {
        for (int j = 20; j < 60; j++) {
            mvaddch(i, j, ' ');
        }
    }
}

void entityString(gameBoard_t *world, cell_t *entity, cell_t *player, char str[])
{
    int x, y;

    x = player->x - entity->x;
    y = player->y - entity->y;

    if (x < 0) {
        if (y < 0) {
            sprintf(str, "%c: %d south and %d east", entity->type, -1 * y, -1 * x);
        } else if (y > 0) {
            sprintf(str, "%c: %d north and %d east", entity->type, y, -1 * x);
        } else {
            sprintf(str, "%c: %d straight east", entity->type, -1 * x);
        }
    } else if (x > 0) {
        if (y < 0) {
            sprintf(str, "%c: %d south and %d west", entity->type, -1 * y, x);
        } else if (y > 0) {
            sprintf(str, "%c: %d north and %d west", entity->type, y, x);
        } else {
            sprintf(str, "%c: %d straight west", entity->type, x);
        }
    } else {
        if (y < 0) {
            sprintf(str, "%c: %d straight south", entity->type, -1 * y);
        } else if (y > 0) {
            sprintf(str, "%c: %d straight north", entity->type, y);
        }
    }
}

int shopMenu(char shopType)
{
    printAmogus();

    mvprintw(18, 26, "Press < to leave!");

    refresh();

    int ch;

    while (true) {
        ch = getch();

        if (ch == '<') {
            return 0;
        }
    }
}

int trainerMenu(gameBoard_t *world, cell_t *player)
{
    set_escdelay(10);

    int start = 0, end = 9, cursorLoc = 2, numTrainers = world->board[world->currY][world->currX]->mh.currLen - 2, lineLoc = 0, trnrIdx = 0;
    char entityStrs[30];

    if (end > numTrainers) {
        end = numTrainers;
    }

    bool inMenu = true;
    menuInit();

    cell_t *trainers[world->board[world->currY][world->currX]->mh.currLen];

    for (int i = 0; i < world->board[world->currY][world->currX]->mh.currLen; i++) {
        if (world->board[world->currY][world->currX]->mh.heap[i].data->type != '@') {
            trainers[trnrIdx] = world->board[world->currY][world->currX]->mh.heap[i].data;

            trnrIdx++;
        }
    }

    for (int i = start; i <= end; i++) {
        entityString(world, trainers[i], player, entityStrs);

        mvprintw(lineLoc + 2, 25, entityStrs);

        lineLoc+=2;
    }

    mvaddch(cursorLoc, 21, '>');

    while (inMenu) {
        refresh();

        int ch = getch();

        if (ch == 27) {
            inMenu = false;
        } else if (ch == KEY_UP) {
            if (cursorLoc == 2) {
                if (start > 0) {
                    cursorLoc = 20; // move cursor back to the bottom

                    end = start - 1;

                    start = end - 9;

                    if (end > numTrainers) {
                        end = numTrainers;
                    }

                    menuInit();

                    lineLoc = 0;

                    for (int i = start; i <= end; i++) {
                        entityString(world, trainers[i], player, entityStrs);

                        mvprintw(lineLoc + 2, 25, entityStrs);

                        lineLoc+=2;
                    }

                    mvaddch(cursorLoc, 21, '>');
                }
            } else {
                mvaddch(cursorLoc, 21, ' ');

                cursorLoc -= 2;

                mvaddch(cursorLoc, 21, '>');
            }
        } else if (ch == KEY_DOWN) {
            if (cursorLoc == 20) {
                if (end < numTrainers) {
                    cursorLoc = 2; // move cursor back to the top

                    start = end + 1;

                    end += 10;

                    if (end > numTrainers) {
                        end = numTrainers;
                    }

                    menuInit();

                    lineLoc = 0;

                    for (int i = start; i <= end; i++) {
                        entityString(world, trainers[i], player, entityStrs);

                        mvprintw(lineLoc + 2, 25, entityStrs);

                        lineLoc+=2;
                    }

                    mvaddch(cursorLoc, 21, '>');
                }
            } else {
                if (mvinch(cursorLoc + 2, 25) != ' ') {
                    mvaddch(cursorLoc, 21, ' ');

                    cursorLoc += 2;

                    mvaddch(cursorLoc, 21, '>');
                }
            }
        }
    }

    set_escdelay(1000);

    return 0;
}

void runGame(gameBoard_t *world, cell_t *player)
{
    char message[50] = "Your move!";
    int result;

    printCurr(world, message);

    bool alive = true;
    while (alive) {
        while (peek(&world->board[world->currY][world->currX]->mh) != player) {
            moveEntity(world->board[world->currY][world->currX], &world->board[world->currY][world->currX]->mh, peek(&world->board[world->currY][world->currX]->mh), player);
        }

        // TODO: make the entities update the buffer as they move so that we don't waste power/time redoing the whole board each time
        //printCurr(world, message);

        mvprintw(0, 0, "Current Location: (%d, %d): %s\n", world->currX, world->currY, message);
        refresh();

        // reset the message
        strcpy(message, "Your move!");

        int ch = getch();

        /// Quit command
        if (ch == 'q') {
            alive = false;

        /// Trainer menu key
        } else if (ch == 't') {
            trainerMenu(world, player);

            printCurr(world, (char *) "bruh");
            sprintf(message, "Left the trainer menu!");

        } else if (ch == '>') {
            if (world->board[world->currY][world->currX]->map[player->y][player->x].type == 'M' || world->board[world->currY][world->currX]->map[player->y][player->x].type == 'C') {
                shopMenu(world->board[world->currY][world->currX]->map[player->y][player->x].type);

                printCurr(world, (char *) "You leave the shop!");
                sprintf(message, "You leave the shop!");
            }

            refresh();

        /// Movement control below
        } else if (ch == '7' || ch == 'y') {
            result = movePlayer(player->y - 1, player->x - 1, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y - 1, player->x - 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
        } else if (ch == '8' || ch == 'k') {
            result = movePlayer(player->y - 1, player->x, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y - 1, player->x);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
        } else if (ch == '9' || ch == 'u') {
            result = movePlayer(player->y - 1, player->x + 1, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y - 1, player->x + 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
        } else if (ch == '4' || ch == 'h') {
            result = movePlayer(player->y, player->x - 1, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y, player->x - 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
        } else if (ch == '5' || ch == ' ') {
            player->dist += determineCost(world->board[world->currY][world->currX]->map[player->y][player->x].type, '@');

            heapifyDown(&world->board[world->currY][world->currX]->mh, 0);

            sprintf(message, "Stood still...");
        } else if (ch == '6' || ch == 'l') {
            result = movePlayer(player->y, player->x + 1, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y, player->x + 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
        } else if (ch == '1' || ch == 'b') {
            result = movePlayer(player->y + 1, player->x - 1, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y + 1, player->x - 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
        } else if (ch == '2' || ch == 'j') {
            result = movePlayer(player->y + 1, player->x, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y + 1, player->x);
            } else if (result == 1) {
                sprintf(message, "Your path is blocked, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
        } else if (ch == '3' || ch == 'n') {
            result = movePlayer(player->y + 1, player->x + 1, world, player, message);

            if (result == -1) {
                sprintf(message, "Invalid move to (%d, %d)", player->y + 1, player->x + 1);
            } else if (result == 1) {
                sprintf(message, "Place occupied, cannot move there!");
            } else if (result == 3) {
                sprintf(message, "You have already defeated this trainer!");
            }
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
        if (!strcmp("pokemon", argv[1])) {
            std::vector<pokemon> pokeList;

            // Here for the file reading
            int x = readAllPokemon(pokeList);

            if (x == -1) {
                fprintf(stderr, "Error: Problem finding pokemon.csv\n");
                return -1;
            }

            for (int i = 0; i < (int) pokeList.size(); i++) {
                std::cout << pokeList[i] << std::endl;
            }

            return 0; // end the game before it starts

        } else if (!strcmp("pokemon_species", argv[1])) {
            std::vector<pokemon_species> pokeList;

            // Here for the file reading
            int x = readAllPokemonSpecies(pokeList);

            if (x == -1) {
                fprintf(stderr, "Error: Problem finding pokemon_species.csv\n");
                return -1;
            }

            for (int i = 0; i < (int) pokeList.size(); i++) {
                std::cout << pokeList[i] << std::endl;
            }

            return 0; // end the game before it starts
        } else if (!strcmp("experience", argv[1])) {
            std::vector<experience> expList;

            int x = readAllExperience(expList);

            if (x == -1) {
                fprintf(stderr, "Error: Problem finding experience.csv\n");
                return -1;
            }

            for (int i = 0; i < (int) expList.size(); i++) {
                std::cout << expList[i] << std::endl;
            }

            return 0;
        } else if (!strcmp("type_names", argv[1])) {
            std::vector<type_names> typList;

            int x = readAllTypes(typList);

            if (x == -1) {
                fprintf(stderr, "Error: Problem finding type_names.csv\n");
                return -1;
            }

            for (int i = 0; i < (int) typList.size(); i++) {
                std::cout << typList[i] << std::endl;
            }

            return 0;
        } else if (!strcmp("pokemon_moves", argv[1])) {
            std::vector<pokemon_moves> mvList;

            int x = readAllPokemonMoves(mvList);

            if (x == -1) {
                fprintf(stderr, "Error: Problem finding type_names.csv\n");
                return -1;
            }

            for (int i = 0; i < (int) mvList.size(); i++) {
                std::cout << mvList[i] << std::endl;
            }

            return 0;
        } else if (!strcmp("moves", argv[1])) {
            std::vector<moves> mvList;

            int x = readAllMoves(mvList);

            if (x == -1) {
                fprintf(stderr, "Error: Problem finding type_names.csv\n");
                return -1;
            }

            for (int i = 0; i < (int) mvList.size(); i++) {
                std::cout << mvList[i] << std::endl;
            }

            return 0;
        } else if (!strcmp("--numtrainers", argv[1]) && argv[2] != NULL) {
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
