//
// Created by hunter on 4/16/22.
//

#include "menus.h"
#include "mapBuilder.h"
#include "pokemon.h"
#include <curses.h>
#include <cmath>

int fightRandomPokemon(int dist, player_cell *player)
{
    set_escdelay(10);

    int cursX = 17, cursY = 15, playerSpeed, escapeOdds, fleeAttempts = 0, critOdds;
    pokemon_entity *playerCurr = player->pkmns[0]; // get the player's first pokemon

    playerSpeed = playerCurr->pkmnStats[5];

    for (int i = 4; i < 19; i++) {
        for (int j = 15; j < 65; j++) {
            mvaddch(i, j, ' ');
        }
    }

    int pID = rand() % pokeList.size() + 1;

    pokemon_entity pe(pokeList, species, expList, types, pkmnMoves, mvList, pkmnStats, statList, dist, pID);

    if (pe.shiny) {
        mvprintw(5, 16, "A shiny level %d wild %s appears!", pe.level, pe.pkm->identifier.c_str());
    } else {
        mvprintw(5, 16, "A level %d wild %s appears!", pe.level, pe.pkm->identifier.c_str());
    }

    mvprintw(6, 16, "It has %d hp, and %d total xp", pe.pkmnStats[0], pe.xp);

    mvprintw(7, 16, "It can attack with: ");

    for (int i = 0; i < (int) pe.currMoves.size(); i++) {
        mvprintw(i+8, 19, "%s", pe.currMoves[i]->identifier.c_str());
    }

    mvprintw(15, 19, "Fight");
    mvprintw(17, 19, "Pokemon");

    mvprintw(15, 39, "Bag");
    mvprintw(17, 39, "Run");

    mvaddch(15, 17, '>');

    refresh();

    int ch;

    while (true) {
        ch = getch();

        if (ch == 27) {
            set_escdelay(1000);
            return 0;
        } else if (ch == KEY_DOWN) {
            if (cursY == 15) {
                mvaddch(cursY, cursX, ' ');

                cursY = 17;

                mvaddch(cursY, cursX, '>');
            }
        } else if (ch == KEY_UP) {
            if (cursY == 17) {
                mvaddch(cursY, cursX, ' ');

                cursY = 15;

                mvaddch(cursY, cursX, '>');
            }
        } else if (ch == KEY_LEFT) {
            if (cursX == 37) {
                mvaddch(cursY, cursX, ' ');

                cursX = 17;

                mvaddch(cursY, cursX, '>');
            }
        } else if (ch == KEY_RIGHT) {
            if (cursX == 17) {
                mvaddch(cursY, cursX, ' ');

                cursX = 37;

                mvaddch(cursY, cursX, '>');
            }
        } else if (ch == ' ') {
            // MENUING by selecting w/ space
            if (cursX == 17 && cursY == 15) {
                // Fight
                mvprintw(22, 0, "Fight!");
            } else if (cursX == 17 && cursY == 17) {
                // Pokemon
                mvprintw(22, 0, "Pokemon");
            } else if (cursX == 37 && cursY == 15) {
                // Bag
                mvprintw(22, 0, "Bag");
            }  else if (cursX == 37 && cursY == 17) {
                // Run
                fleeAttempts++;

                escapeOdds = floor(((playerSpeed * 32) / ((int) (floor(pe.pkmnStats[5] / 4)) % 256))) + 30 * fleeAttempts;

                if (escapeOdds > 255 || ((int) (pe.pkmnStats[5] / 4)) % 256 == 0) {
                    return 0;
                } else if (rand() % 256 < escapeOdds) {
                    return 0;
                }
                // If not, keep looping
            }
        }
    }
}

// Makes the black box so that our menus are all the same size
void menuInit() {
    for (int i = 2; i < 21; i++) {
        for (int j = 20; j < 60; j++) {
            mvaddch(i, j, ' ');
        }
    }
}

void entityString(world *world, entity_cell *entity, player_cell *player, char str[])
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

int trainerMenu(world *world, player_cell *player)
{
    set_escdelay(10);

    int start = 0, end = 9, cursorLoc = 2, numTrainers = world->board[world->currY][world->currX]->mh.currLen - 2, lineLoc = 0, trnrIdx = 0;
    char entityStrs[30];

    if (end > numTrainers) {
        end = numTrainers;
    }

    bool inMenu = true;
    menuInit();

    cell *trainers[world->board[world->currY][world->currX]->mh.currLen];

    for (int i = 0; i < world->board[world->currY][world->currX]->mh.currLen; i++) {
        if (world->board[world->currY][world->currX]->mh.heap[i].data->type != '@') {
            trainers[trnrIdx] = world->board[world->currY][world->currX]->mh.heap[i].data;

            trnrIdx++;
        }
    }

    for (int i = start; i <= end; i++) {
        entityString(world, (entity_cell *) trainers[i], player, entityStrs);

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
                        entityString(world, (entity_cell *) trainers[i], player, entityStrs);

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
                        entityString(world, (entity_cell *) trainers[i], player, entityStrs);

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

void bagMenu(player_cell *player) {
    set_escdelay(10);

    bool quit = false;

    for (int i = 3; i < 19; i++) {
        for (int j = 20; j < 60; j++) {
            mvaddch(i, j, ' ');
        }
    }

    mvprintw(3, 21, "Bag: Esc to exit");

    mvprintw(5, 22, "Pokeballs: %dx", player->pokeballs);
    mvprintw(6, 22, "Potions: %dx", player->potions);
    mvprintw(7, 22, "Revives: %dx", player->revives);

    mvprintw(9, 21, "Pokemon: ");

    for (int i = 0; i < player->pkmnCnt; i++) {
        mvprintw(10 + i, 22, ("lvl " + std::to_string(player->pkmns[i]->level) + " " + player->pkmns[i]->pkm->identifier + " " + std::to_string(player->pkmns[i]->pkmnStats[0]) + " HP").c_str());
    }

    refresh();

    while (!quit) {
        int ch = getch();

        if (ch == 27) {
            set_escdelay(1000); // reset the delay
            quit = true;
        }
    }
}

void startScreen(player_cell *player) {
    pokemon_entity *choices[3];

    int p1 = rand() % 898 + 1, p2 = rand() % 898 + 1, p3 = rand() % 898 + 1;

    choices[0] = new pokemon_entity(pokeList, species, expList, types, pkmnMoves, mvList, pkmnStats, statList, 1, p1);
    choices[1] = new pokemon_entity(pokeList, species, expList, types, pkmnMoves, mvList, pkmnStats, statList, 1, p2);
    choices[2] = new pokemon_entity(pokeList, species, expList, types, pkmnMoves, mvList, pkmnStats, statList, 1, p3);

    mvprintw(6, 30, "Choose your starter w/ space!");

    mvprintw(10, 13 - (choices[0]->pkm->identifier.length() / 2), choices[0]->pkm->identifier.c_str());
    mvprintw(10, 39 - (choices[1]->pkm->identifier.length() / 2), choices[1]->pkm->identifier.c_str());
    mvprintw(10, 65 - (choices[2]->pkm->identifier.length() / 2), choices[2]->pkm->identifier.c_str());

    bool choosing = true;

    int currPkmn = 0, pkmnIdx = 13;

    mvaddch(9, pkmnIdx, ACS_DIAMOND);

    while (choosing) {
        int ch = getch();

        refresh();

        switch (ch) {
            case ' ':
                player->pkmns[0] = choices[currPkmn];
                player->pkmnCnt++;

                choosing = false;
                break;

            case KEY_LEFT:
                if (currPkmn != 0) {
                    currPkmn--;

                    mvaddch(9, pkmnIdx, ' ');

                    pkmnIdx -= 26;

                    mvaddch(9, pkmnIdx, ACS_DIAMOND);
                }
                break;

            case KEY_RIGHT:
                if (currPkmn != 2) {
                    currPkmn++;

                    mvaddch(9, pkmnIdx, ' ');

                    pkmnIdx += 26;

                    mvaddch(9, pkmnIdx, ACS_DIAMOND);
                }
                break;
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

int fightPLayer(world *world, entity_cell *entity, player_cell *player)
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
            printCurr(world);

            set_escdelay(1000);
            return 0;
        }
    }
}