//
// Created by hunter on 4/16/22.
//

#include "menus.h"
#include "mapBuilder.h"
#include "pokemon.h"
#include <curses.h>
#include <cmath>

void drawPokemonFightMenu(player_cell *player, pokemon_entity *pe, int pCurrpkmn) {
    for (int i = 3; i < 19; i++) {
        for (int j = 15; j < 65; j++) {
            mvaddch(i, j, ' ');
        }
    }

    if (pe->shiny) {
        mvprintw(4, 16, "A shiny level %d wild %s appears!", pe->level, pe->pkm->identifier.c_str());
    } else {
        mvprintw(4, 16, "A level %d wild %s appears!", pe->level, pe->pkm->identifier.c_str());
    }

    mvprintw(5, 16, "========== %d hp", pe->pkmnStats[0]);

    mvprintw(9, 16, "You have a level %d %s", player->pkmns[pCurrpkmn]->level, player->pkmns[pCurrpkmn]->pkm->identifier.c_str());
    mvprintw(10, 16, "========== %d hp", player->pkmns[pCurrpkmn]->pkmnStats[0]);
    //mvprintw(11, 16, "========== %d xp", player->pkmns[pCurrpkmn]->xp);

    mvprintw(15, 19, "Fight");
    mvprintw(17, 19, "Pokemon");

    mvprintw(15, 39, "Bag");
    mvprintw(17, 39, "Run");

    mvaddch(15, 17, '>');
}

void fightRandomPokemon(int dist, player_cell *player)
{
    int cursX = 17, cursY = 15, playerSpeed, escapeOdds, fleeAttempts = 0, critOdds, pCurrpkmn = 0;
    int pDmg, eDmg, pPrio, ePrio;
    pokemon_entity *playerCurr = player->pkmns[0]; // get the player's first pokemon

    playerSpeed = playerCurr->pkmnStats[5];

    int pID = rand() % pokeList.size() + 1;

    pokemon_entity *pe = new pokemon_entity(pokeList, species, expList, types, pkmnMoves, mvList, pkmnStats, statList, dist, pID);

    drawPokemonFightMenu(player, pe, pCurrpkmn);

    refresh();

    int ch;
    bool fightOn = true;

    while (fightOn) {
        pDmg = 0;
        eDmg = 0;
        pPrio = 0;
        ePrio = 0;

        ch = getch();

        if (ch == KEY_DOWN) {
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
            // Wipe the message slot underneath
            mvprintw(22, 0, "                                                  ");

            // MENUING by selecting w/ space
            if (cursX == 17 && cursY == 15) { // Fight
                int cursor = 0;

                // Clear the menu
                for (int i = 15; i < 18; i++) {
                    for (int j = 17; j < 50; j++) {
                        mvaddch(i, j, ' ');
                    }
                }

                // print the fight menu
                for (int i = 0; i < player->pkmns[pCurrpkmn]->currMoves.size(); i++) {
                    mvprintw(14 + i, 19, player->pkmns[pCurrpkmn]->currMoves[i]->identifier.c_str());

                }

                int fightCurs = 0;

                mvaddch(14 + fightCurs, 17, '>');

                bool moved = false;

                while (!moved) {
                    int ch4 = getch();

                    switch (ch4) {
                        case ' ':
                            //pDmg = player->pkmns[pCurrpkmn]->currMoves[fightCurs]->power;
                            int crit;

                            // See if this is a crit or not
                            if (rand() % 255 < (player->pkmns[pCurrpkmn]->pkmnBaseStats[6] / 2)) {
                                crit = 1.5;
                            } else {
                                crit = 1;
                            }

                            if (rand() % 100 < player->pkmns[pCurrpkmn]->currMoves[fightCurs]->accuracy) {
                                pDmg = ((((2 * player->pkmns[pCurrpkmn]->level) + 2) * player->pkmns[pCurrpkmn]->currMoves[fightCurs]->power * (player->pkmns[pCurrpkmn]->pkmnStats[1] / player->pkmns[pCurrpkmn]->pkmnStats[2]) / 50) + 2) * crit * (rand() % 15 + 85);
                            }

                            moved = true;
                            break;

                        case KEY_DOWN:
                            if (fightCurs < player->pkmns[pCurrpkmn]->currMoves.size() - 1) {
                                mvaddch(14 + fightCurs, 17, ' ');

                                fightCurs++;

                                mvaddch(14 + fightCurs, 17, '>');
                            }
                            break;

                        case KEY_UP:
                            if (fightCurs > 0) {
                                mvaddch(14 + fightCurs, 17, ' ');

                                fightCurs--;

                                mvaddch(14 + fightCurs, 17, '>');
                            }
                            break;
                    }
                }

                drawPokemonFightMenu(player, pe, pCurrpkmn);
                cursX = 17;
                cursY = 15;

            } else if (cursX == 17 && cursY == 17) { // Pokemon
                int pkIdx = 0;
                int i;

                // Clear the menu
                for (i = 15; i < 18; i++) {
                    for (int j = 17; j < 50; j++) {
                        mvaddch(i, j, ' ');
                    }
                }

                // List the player's pokemon for choosing
                for (i = 0; i < player->pkmnCnt; i++) {
                    mvprintw(i+12, 20, (player->pkmns[i]->pkm->identifier + " HP: " + std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
                }
                mvprintw(i+12, 20, "Back");

                mvaddch(12 + pkIdx, 18, '>');

                bool chosen = false;

                while (!chosen) {
                    int ch2 = getch();

                    switch (ch2) {
                        case KEY_UP:
                            if (pkIdx > 0) {
                                mvaddch(12 + pkIdx, 18, ' ');

                                pkIdx--;

                                mvaddch(12 + pkIdx, 18, '>');
                            }
                            break;

                        case KEY_DOWN:
                            if (pkIdx < player->pkmnCnt) {
                                mvaddch(12 + pkIdx, 18, ' ');

                                pkIdx++;

                                mvaddch(12 + pkIdx, 18, '>');
                            }
                            break;

                        case ' ':
                            // Back out of the menu if you change your mind
                            if (pkIdx == player->pkmnCnt) {
                                chosen = true;
                                break;
                            }

                            if (player->pkmns[pkIdx]->pkmnStats[0] != 0) {
                                pCurrpkmn = pkIdx;
                                chosen = true;
                            } else {
                                mvprintw(22, 0, "That Pokemon has fainted! Choose another!");
                            }
                            break;
                    }
                }

                // Clear the menu
                for (int i = 15; i < 18; i++) {
                    for (int j = 17; j < 50; j++) {
                        mvaddch(i, j, ' ');
                    }
                }

                drawPokemonFightMenu(player, pe, pCurrpkmn);
                cursX = 17;
                cursY = 15;
        } else if (cursX == 37 && cursY == 15) { // Bag
            // Clear the menu
            for (int i = 15; i < 18; i++) {
                for (int j = 17; j < 50; j++) {
                    mvaddch(i, j, ' ');
                }
            }

            int bagCursor = 0, bagX = 18;

            mvprintw(12, 20, "PokeBalls %dx", player->pokeballs);
            mvprintw(13, 20, "Potions: %dx", player->potions);
            mvprintw(14, 20, "Revives: %dx", player->revives);
            mvprintw(15, 20, "Cancel");

            // List the player's pokemon for choosing
            int i;
            for (i = 0; i < player->pkmnCnt; i++) {
                mvprintw(i+12, 40, (player->pkmns[i]->pkm->identifier + " HP: " + std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
            }

                mvprintw(i+12, 40, "Back");

            mvaddch(12, bagX, '>');

            bool done = false;

            while (!done) {
                int ch2 = getch();

                switch (ch2) {
                    case 27:
                        done = true;
                        break;

                    case KEY_DOWN:
                        if (bagCursor < 3) {
                            mvaddch(12 + bagCursor, bagX, ' ');

                            bagCursor++;

                            mvaddch(12 + bagCursor, bagX, '>');
                        }
                        break;

                    case KEY_UP:
                        if (bagCursor > 0) {
                            mvaddch(12 + bagCursor, bagX, ' ');

                            bagCursor--;

                            mvaddch(12 + bagCursor, bagX, '>');
                        }
                        break;


                    case ' ':
                        switch (bagCursor) {
                            case 0: // Pokeball
                                if (player->pokeballs > 0) {
                                    if (player->pkmnCnt < 6) {
                                        player->pkmns[player->pkmnCnt] = pe;
                                        player->pkmnCnt++;

                                        player->pokeballs--;

                                        done = true;
                                        fightOn = false;
                                    }
                                }
                                break;

                            case 1: // Potions
                                if (player->potions > 0) {
                                    // Swap over to the pokemon list to choose who gets the potion
                                    mvaddch(12+bagCursor, bagX, ' ');

                                    bagCursor = 0;
                                    bagX = 38;

                                    mvaddch(12+bagCursor, bagX, '>');

                                    bool potioned = false;

                                    while (!potioned) {
                                        int ch3 = getch();

                                        switch (ch3) {
                                            case KEY_DOWN:
                                                if (bagCursor < player->pkmnCnt) {
                                                    mvaddch(12 + bagCursor, bagX, ' ');

                                                    bagCursor++;

                                                    mvaddch(12 + bagCursor, bagX, '>');
                                                }
                                                break;

                                            case KEY_UP:
                                                if (bagCursor > 0) {
                                                    mvaddch(12 + bagCursor, bagX, ' ');

                                                    bagCursor--;

                                                    mvaddch(12 + bagCursor, bagX, '>');
                                                }
                                                break;

                                            case ' ':
                                                if (bagCursor == player->pkmnCnt) {
                                                    mvaddch(12 + bagCursor, bagX, ' ');

                                                    bagCursor = 0;
                                                    bagX = 18;

                                                    mvaddch(12 + bagCursor, bagX, '>');

                                                    potioned = true;
                                                    done = true;
                                                } else if (player->pkmns[bagCursor]->pkmnStats[0] > 0 && player->pkmns[bagCursor]->pkmnStats[0] < setPkmnHP(player->pkmns[bagCursor]->IVs[0], player->pkmns[bagCursor]->pkmnBaseStats[0], player->pkmns[bagCursor]->level)) {
                                                    // Give the selected pokemon either it's max hp or 20 more than it has right now
                                                    player->pkmns[bagCursor]->pkmnStats[0] = std::min(player->pkmns[bagCursor]->pkmnStats[0] += 20, setPkmnHP(player->pkmns[bagCursor]->IVs[0], player->pkmns[bagCursor]->pkmnBaseStats[0], player->pkmns[bagCursor]->level));
                                                    player->potions--;

                                                    bagCursor = 0;
                                                    bagX = 18;

                                                    potioned = true;
                                                    done = true;
                                                }
                                                break;
                                        }
                                    }
                                }
                                break;

                            case 2: // Revives
                                if (player->revives > 0) {
                                    // Swap over to the pokemon list to choose who gets the potion
                                    mvaddch(12+bagCursor, bagX, ' ');

                                    bagCursor = 0;
                                    bagX = 38;

                                    mvaddch(12+bagCursor, bagX, '>');

                                    bool revived = false;

                                    while (!revived) {
                                        int ch3 = getch();

                                        switch (ch3) {
                                            case KEY_DOWN:
                                                if (bagCursor < player->pkmnCnt) {
                                                    mvaddch(12 + bagCursor, bagX, ' ');

                                                    bagCursor++;

                                                    mvaddch(12 + bagCursor, bagX, '>');
                                                }
                                                break;

                                            case KEY_UP:
                                                if (bagCursor > 0) {
                                                    mvaddch(12 + bagCursor, bagX, ' ');

                                                    bagCursor--;

                                                    mvaddch(12 + bagCursor, bagX, '>');
                                                }
                                                break;

                                            case ' ':
                                                if (bagCursor == player->pkmnCnt) {
                                                    mvaddch(12 + bagCursor, bagX, ' ');

                                                    bagCursor = 0;
                                                    bagX = 18;

                                                    mvaddch(12 + bagCursor, bagX, '>');
                                                    revived = true;

                                                } else if (player->pkmns[bagCursor]->pkmnStats[0] == 0) {
                                                    // Give the selected pokemon either it's max hp or 20 more than it has right now
                                                    player->pkmns[bagCursor]->pkmnStats[0] = (int) setPkmnHP(player->pkmns[bagCursor]->IVs[0], player->pkmns[bagCursor]->pkmnBaseStats[0], player->pkmns[bagCursor]->level) / 2;
                                                    player->revives--;

                                                    bagCursor = 0;
                                                    bagX = 18;

                                                    revived = true;
                                                    done = true;
                                                }
                                                break;
                                        }
                                    }
                                }
                                break;

                            case 3:
                                done = true;
                                break;
                        }
                    break;
                }
            }

            // Reset
            drawPokemonFightMenu(player, pe, pCurrpkmn);
            cursX = 17;
            cursY = 15;
        } else if (cursX == 37 && cursY == 17) { // Run
            fleeAttempts++;

            escapeOdds = floor(((playerSpeed * 32) / ((int) (floor(pe->pkmnStats[5] / 4)) % 256))) + 30 * fleeAttempts;

            if (escapeOdds > 255 || ((int) (pe->pkmnStats[5] / 4)) % 256 == 0) {
                delete pe; // since we didn't catch it, free the memory
                fightOn = false;
            } else if (rand() % 256 < escapeOdds) {
                delete pe;
                fightOn = false;
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