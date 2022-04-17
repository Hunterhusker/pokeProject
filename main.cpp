#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curses.h>
#include "mapBuilder.h"
#include "heatMap.h"
#include "minHeap.h"
#include "fileReader.h"
#include "pokemon.h"
#include "menus.h"
#include "world.h"
#include <vector>
#include <cmath>

// Some global values to use here in main
std::vector<pokemon> pokeList;
std::vector<pokemon_species> species;
std::vector<experience> expList;
std::vector<type_names> types;
std::vector<pokemon_moves> pkmnMoves;
std::vector<moves> mvList;
std::vector<pokemon_stats> pkmnStats;
std::vector<stats> statList;

void cursesInit()
{
    initscr();

    raw();
    noecho();
    curs_set(0);

    keypad(stdscr, TRUE);
}

void runGame(world *world, player_cell *player)
{
    char message[50] = "Your move!";
    int result;

    printCurr(world);

    bool alive = true;
    while (alive) {
        while (peek(&world->board[world->currY][world->currX]->mh) != player) {
            moveEntity(world, world->board[world->currY][world->currX], &world->board[world->currY][world->currX]->mh, (entity_cell *) peek(&world->board[world->currY][world->currX]->mh), player);
        }

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

            printCurr(world);
            sprintf(message, "Left the trainer menu!");

        } else if (ch == 'b') {
            bagMenu(player);

            printCurr(world);
        } else if (ch == '>') {
            if (world->board[world->currY][world->currX]->map[player->y][player->x].type == 'M' || world->board[world->currY][world->currX]->map[player->y][player->x].type == 'C') {
                shopMenu(world->board[world->currY][world->currX]->map[player->y][player->x].type);

                printCurr(world);
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

            if (world->board[world->currY][world->currX]->map[player->y][player->x].type == ':') {
                int fight = rand() % 10;

                if (fight == 0) {
                    fightRandomPokemon(abs(world->currX - 199) + abs(world->currY - 199), player);

                    printCurr(world);
                    sprintf(message, "You beat the pokemon!");
                }
            }

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

// Populate the globals with the DB info
int readPokeDB() {
    int success;

    success = readAllPokemon(pokeList);

    if (success == -1) {return -1;}

    success = readAllPokemonSpecies(species);

    if (success == -1) {return -1;}

    success = readAllExperience(expList);

    if (success == -1) {return -1;}

    success = readAllTypes(types);

    if (success == -1) {return -1;}

    success = readAllPokemonMoves(pkmnMoves);

    if (success == -1) {return -1;}

    success = readAllMoves(mvList);

    if (success == -1) {return -1;}

    success = readAllPokemonStats(pkmnStats);

    if (success == -1) {return -1;}

    success = readAllStats(statList);

    if (success == -1) {return -1;}

    return 0;
}

int main(int argc, char *argv[])
{
    world world;
    player_cell *player; // a pointer to the player for better access
    int trainerCnt = 10;

    // Read the DB into this mess
    int s = readPokeDB();

    if (s == -1) {
        fprintf(stderr, "Error finding the required csv files\n");
        return -1;
    }

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
    player = placePlayer(world.board[world.currY][world.currX], &world.board[world.currY][world.currX]->mh);

    startScreen(player);

    runGame(&world, player);

    endwin();

    destroyWorld(&world); // must be run to collect garbage at the end

    return 0;
}
