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

int fightRandomPokemon(int dist, player_cell *player)
{
    int cursX = 17, cursY = 15, playerSpeed, escapeOdds, fleeAttempts = 0, pCurrpkmn = 0;
    int pDmg, eDmg, pPrio, ePrio;

    bool captured = false;

    playerSpeed = player->pkmns[pCurrpkmn]->pkmnStats[5];

    int pID = rand() % pokeList.size() + 1;

    // If the first pokemon is fainted, grab another
    if (player->pkmns[pCurrpkmn]->pkmnStats[0] == 0) {
        // Check all other pokemon for signs of life
        for (int i = 1; i < player->pkmnCnt; i++) {
            if (player->pkmns[i]->pkmnStats[0] > 0) {
                pCurrpkmn = i;
                break;
            }
        }

        // If we didn't update the current pokemon, then leave before things get too waccy
        if (pCurrpkmn == 0) {
            return -1;
        }
    }

    pokemon_entity *pe = new pokemon_entity(dist, pID);

    drawPokemonFightMenu(player, pe, pCurrpkmn);

    refresh();

    int ch;
    bool fightOn = true;

    while (fightOn) {

        bool playerTurn = true;

        while (playerTurn) {
            pDmg = 0;
            eDmg = 0;
            pPrio = 6; // Init this to be 6 so that if we don't choose an attack we have max priority
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
                    // Clear the menu
                    for (int i = 15; i < 18; i++) {
                        for (int j = 17; j < 50; j++) {
                            mvaddch(i, j, ' ');
                        }
                    }

                    // print the fight menu
                    for (int i = 0; i < (int) player->pkmns[pCurrpkmn]->currMoves.size(); i++) {
                        mvprintw(14 + i, 19, (player->pkmns[pCurrpkmn]->currMoves[i]->identifier + " " +
                                              std::to_string(player->pkmns[pCurrpkmn]->PPList[i]) + "/" +
                                              std::to_string(player->pkmns[pCurrpkmn]->currMoves[i]->pp)).c_str());
                    }

                    int fightCurs = 0;

                    mvaddch(14 + fightCurs, 17, '>');

                    bool moved = false;

                    while (!moved) {
                        int ch4 = getch();

                        switch (ch4) {
                            case ' ':
                                if (player->pkmns[pCurrpkmn]->PPList[fightCurs] <= 0) {
                                    break; // don't let players use moves w/o PP
                                } else {
                                    player->pkmns[pCurrpkmn]->PPList[fightCurs]--; // if we have PP to spare, decrement it to use it
                                }

                                int crit, stab;
                                crit = 1;
                                stab = 1;

                                // See if this is a crit or not
                                if (rand() % 255 < (player->pkmns[pCurrpkmn]->pkmnBaseStats[6] / 2)) {
                                    crit = 1.5;
                                }

                                for (int i = 0; i < (int) player->pkmns[pCurrpkmn]->pkmnTypeList.size(); i++) {
                                    if (player->pkmns[pCurrpkmn]->pkmnTypeList[i]->type_id ==
                                        player->pkmns[pCurrpkmn]->currMoves[fightCurs]->type_id) {
                                        stab = 1.5;
                                    }
                                }

                                if (rand() % 100 < player->pkmns[pCurrpkmn]->currMoves[fightCurs]->accuracy ||
                                    player->pkmns[pCurrpkmn]->currMoves[fightCurs]->accuracy == -1) {
                                    if (player->pkmns[pCurrpkmn]->currMoves[fightCurs]->power != -1) {
                                        pDmg = ((((2 * player->pkmns[pCurrpkmn]->level) + 2) *
                                                 player->pkmns[pCurrpkmn]->currMoves[fightCurs]->power *
                                                 (player->pkmns[pCurrpkmn]->pkmnStats[1] /
                                                  player->pkmns[pCurrpkmn]->pkmnStats[2]) / 50) + 2) * crit *
                                               ((double) (rand() % 15 + 85) / 100) * stab;
                                    } else {
                                        pDmg = 0;
                                    }

                                    pPrio = player->pkmns[pCurrpkmn]->currMoves[fightCurs]->priority;
                                }

                                moved = true;
                                playerTurn = false;
                                break;

                            case KEY_DOWN:
                                if (fightCurs < (int) player->pkmns[pCurrpkmn]->currMoves.size() - 1) {
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
                        mvprintw(i + 12, 20, (player->pkmns[i]->pkm->identifier + " HP: " +
                                              std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
                    }
                    mvprintw(i + 12, 20, "Back");

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
                                    playerTurn = false;
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
                        mvprintw(i + 12, 40, (player->pkmns[i]->pkm->identifier + " HP: " +
                                              std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
                    }

                    mvprintw(i + 12, 40, "Back");

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
                                                playerTurn = false;
                                                captured = true;
                                            }
                                        }
                                        break;

                                    case 1: // Potions
                                        if (player->potions > 0) {
                                            // Swap over to the pokemon list to choose who gets the potion
                                            mvaddch(12 + bagCursor, bagX, ' ');

                                            bagCursor = 0;
                                            bagX = 38;

                                            mvaddch(12 + bagCursor, bagX, '>');

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
                                                        } else if (player->pkmns[bagCursor]->pkmnStats[0] > 0 &&
                                                                   player->pkmns[bagCursor]->pkmnStats[0] <
                                                                   setPkmnHP(player->pkmns[bagCursor]->IVs[0],
                                                                             player->pkmns[bagCursor]->pkmnBaseStats[0],
                                                                             player->pkmns[bagCursor]->level)) {
                                                            // Give the selected pokemon either it's max hp or 20 more than it has right now
                                                            player->pkmns[bagCursor]->pkmnStats[0] = std::min(
                                                                    player->pkmns[bagCursor]->pkmnStats[0] += 20,
                                                                    setPkmnHP(player->pkmns[bagCursor]->IVs[0],
                                                                              player->pkmns[bagCursor]->pkmnBaseStats[0],
                                                                              player->pkmns[bagCursor]->level));
                                                            player->potions--;

                                                            bagCursor = 0;
                                                            bagX = 18;

                                                            potioned = true;
                                                            done = true;
                                                            playerTurn = false;
                                                        }
                                                        break;
                                                }
                                            }
                                        }
                                        break;

                                    case 2: // Revives
                                        if (player->revives > 0) {
                                            // Swap over to the pokemon list to choose who gets the potion
                                            mvaddch(12 + bagCursor, bagX, ' ');

                                            bagCursor = 0;
                                            bagX = 38;

                                            mvaddch(12 + bagCursor, bagX, '>');

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
                                                            player->pkmns[bagCursor]->pkmnStats[0] =
                                                                    (int) setPkmnHP(player->pkmns[bagCursor]->IVs[0],
                                                                                    player->pkmns[bagCursor]->pkmnBaseStats[0],
                                                                                    player->pkmns[bagCursor]->level) /
                                                                    2;
                                                            player->revives--;

                                                            bagCursor = 0;
                                                            bagX = 18;

                                                            revived = true;
                                                            done = true;
                                                            playerTurn = false;
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

                    escapeOdds = floor(((playerSpeed * 32) / ((int) (floor(pe->pkmnStats[5] / 4)) % 256))) +
                                 30 * fleeAttempts;

                    if (escapeOdds > 255 || ((int) (pe->pkmnStats[5] / 4)) % 256 == 0) {
                        delete pe; // since we didn't catch it, free the memory
                        fightOn = false;
                        playerTurn = false;
                        mvprintw(22, 0, "                                                  ");
                        return 0;
                    } else if (rand() % 256 < escapeOdds) {
                        delete pe;
                        fightOn = false;
                        playerTurn = false;
                        mvprintw(22, 0, "                                                  ");
                        return 0;
                    }

                    playerTurn = false; // Skip the turn if we fail to run
                }
            }
        }

        /// Wild pokemon "AI"

        if (fightOn) {
            int move = rand() % pe->currMoves.size(); // Choose a random move from the list of moves
            int eCrit = 1, eStab = 1;

            // See if this is a crit or not
            if (rand() % 255 < (pe->pkmnBaseStats[6] / 2)) {
                eCrit = 1.5;
            }

            // If we attack w/ a matching move update it to 1.5
            for (int i = 0; i < (int) pe->pkmnTypeList.size(); i++) {
                if (pe->pkmnTypeList[i]->type_id == pe->currMoves[move]->type_id) {
                    eStab = 1.5;
                    break;
                }
            }

            if (rand() % 100 < pe->currMoves[move]->accuracy || pe->currMoves[move]->accuracy == -1) {
                if (pe->currMoves[move]->power != -1) {
                    eDmg = ((((2 * pe->level) + 2) * pe->currMoves[move]->power * (pe->pkmnStats[1] / pe->pkmnStats[2]) / 50) + 2) * eCrit * ((double) (rand() % 15 + 85) / 100) * eStab;

                    mvprintw(22, 0, "%s uses %s for %d damage!", pe->pkm->identifier.c_str(), pe->currMoves[move]->identifier.c_str(), eDmg);
                } else {
                    eDmg = 0;
                    mvprintw(22, 0, "%s uses %s, but misses!", pe->pkm->identifier.c_str(), pe->currMoves[move]->identifier.c_str());
                }

                ePrio = pe->currMoves[move]->priority;
            }

            if (ePrio > pPrio) { // Greater goes first, so then the enemy hits first
                player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);

                // If the player's pokemon lived through it, hit back
                if (player->pkmns[pCurrpkmn]->pkmnStats[0] != 0) {
                    pe->pkmnStats[0] = std::max(0, pe->pkmnStats[0] - pDmg);
                }
            } else if (pPrio > ePrio) { // Player has higher prio
                pe->pkmnStats[0] = std::max(0, pe->pkmnStats[0] - pDmg);

                // If the wild pokemon didn't die from that, then it can attack us
                if (pe->pkmnStats[0] != 0) {
                    player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);
                }
            } else { // The same??? Idk random chance ffs why is this game so complicated
                int who = rand() % 2;

                if (who == 1) { // let the wild pokemon go first
                    player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);

                    // If the player's pokemon lived through it, hit back
                    if (player->pkmns[pCurrpkmn]->pkmnStats[0] != 0) {
                        pe->pkmnStats[0] = std::max(0, pe->pkmnStats[0] - pDmg);
                    }
                } else { // we go first
                    pe->pkmnStats[0] = std::max(0, pe->pkmnStats[0] - pDmg);

                    // If the wild pokemon didn't die from that, then it can attack us
                    if (pe->pkmnStats[0] != 0) {
                        player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);
                    }
                }
            }

            drawPokemonFightMenu(player, pe, pCurrpkmn);
        }

        if (player->pkmns[pCurrpkmn]->pkmnStats[0] <= 0) {
            // Clear the menu
            for (int i = 15; i < 18; i++) {
                for (int j = 17; j < 50; j++) {
                    mvaddch(i, j, ' ');
                }
            }

            // Check for a valid pokemon
            bool hasMorePokemon = false;

            for (int i = 0; i < player->pkmnCnt; i++) {
                if (player->pkmns[i]->pkmnStats[0] > 0) {
                    hasMorePokemon = true;
                    break;
                }
            }

            if (!hasMorePokemon) {
                delete pe;
                mvprintw(22, 0, "                                                  ");
                return -1;
            }

            int faintCurs = 0;

            // Print out their pokemon
            for (int i = 0; i < player->pkmnCnt; i++) {
                mvprintw(12 + i, 20, (player->pkmns[i]->pkm->identifier + " HP: " + std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
            }

            mvaddch(12 + faintCurs, 18, '>');
            bool newPkmn = false;

            while (!newPkmn) {
                int ch5 = getch();

                switch (ch5) {
                    case ' ':
                        if (player->pkmns[faintCurs]->pkmnStats[0] != 0) {
                            pCurrpkmn = faintCurs;
                            newPkmn = true;
                            drawPokemonFightMenu(player, pe, pCurrpkmn);
                        }

                        break;

                    case KEY_DOWN:
                        if (faintCurs < player->pkmnCnt - 1) {
                            mvaddch(12 + faintCurs, 18, ' ');

                            faintCurs++;

                            mvaddch(12 + faintCurs, 18, '>');
                        }

                        break;

                    case KEY_UP:
                        if (faintCurs > 0) {
                            mvaddch(12 + faintCurs, 18, ' ');

                            faintCurs--;

                            mvaddch(12 + faintCurs, 18, '>');
                        }

                        break;
                }
            }
        }

        // If the wild pokemon is dead, then leave
        if (pe->pkmnStats[0] <= 0) {
            //delete pe;
            mvprintw(22, 0, "                                                  ");
            fightOn = false;
            //return 0;
        }
    }

    // Calculate XP earned (for simplicity it will be only to current Pokemon)
    // Crude implementation of formula from https://bulbapedia.bulbagarden.net/wiki/Experience#Experience_gain_in_battle
    int xp = (pe->pkm->base_experience * pe->level) / 7;

    player->pkmns[pCurrpkmn]->xp += xp;
    player->pkmns[pCurrpkmn]->levelup(); // try a level up

    if (!captured) {
        // delete the pokemon if we didnt capture it
        delete pe;
    }

    // Now that that hell is over, we should probably leave the function now
    mvprintw(22, 0, "                                                  ");
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

    int p1 = 1, p2 = 4, p3 = 7; // changed to be the classic starters

    choices[0] = new pokemon_entity(5.0, p1);
    choices[1] = new pokemon_entity(5.0, p2);
    choices[2] = new pokemon_entity(5.0, p3);

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

void pokeMartMenu(int dist) {
    double multiplier; // Create our multiplier var, since scarcity means higher prices
    int ballCost, potionCost, reviveCost; // make vars for different item costs
    int baseBallCost = 20, basePotionCost = 15, baseReviveCost = 25;

    multiplier = (dist * 1.5) / 200 + 1; // Some quick math to map the distance to a price multiplier from 1 to 2.5 based on dist from 0 to 200

    // Set the cost vars
    ballCost = (int) baseBallCost * multiplier;
    potionCost = (int) basePotionCost * multiplier;
    reviveCost = (int) baseReviveCost * multiplier;

    // print the menu

    // Take input
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

void drawTrainerFightMenu(player_cell *player, pokemon_entity *pe, int pCurrpkmn) {
    for (int i = 3; i < 19; i++) {
        for (int j = 15; j < 65; j++) {
            mvaddch(i, j, ' ');
        }
    }

    if (pe->shiny) {
        mvprintw(4, 16, "Trainer deploys a shiny level %d %s!", pe->level, pe->pkm->identifier.c_str());
    } else {
        mvprintw(4, 16, "Trainer deploys a level %d %s", pe->level, pe->pkm->identifier.c_str());
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

void refreshBuffer(world *world) {
    for (int i = 2; i < 18; i++) {
        for (int j = 15; j < 65; j++) {
            if (world->board[world->currY][world->currX]->eMap[i][j] != NULL) {
                mvaddch(i+1, j, world->board[world->currY][world->currX]->eMap[i][j]->type);
            } else {
                mvaddch(i+1, j, world->board[world->currY][world->currX]->map[i][j].type);
            }
        }
    }
}

int fightPLayer(world *world, entity_cell *entity, player_cell *player)
{
    int cursX = 17, cursY = 15, playerSpeed, escapeOdds, fleeAttempts = 0, pCurrpkmn = 0, eCurrPkmn = 0, xp = 0;
    int pDmg, eDmg, pPrio, ePrio;

    playerSpeed = player->pkmns[pCurrpkmn]->pkmnStats[5];

    // If the first pokemon is fainted, grab another
    if (player->pkmns[pCurrpkmn]->pkmnStats[0] == 0) {
        // Check all other pokemon for signs of life
        for (int i = 1; i < player->pkmnCnt; i++) {
            if (player->pkmns[i]->pkmnStats[0] > 0) {
                pCurrpkmn = i;
                break;
            }
        }

        // If we didn't update the current pokemon, then leave before things get too waccy
        if (pCurrpkmn == 0) {
            return -1;
        }
    }

    int dist = abs(world->currX - 199) + abs(world->currY - 199);

    int ePkmnCnt = player->pkmnCnt;

    if (rand() % 5 < 3) {
        ePkmnCnt = std::min(6, ePkmnCnt + 1);
    }

    std::vector<pokemon_entity *> ePkmn;

    //pokemon_entity *pe = new pokemon_entity(dist, pID);

    for (int i = 0; i < ePkmnCnt; i++) {
        int pID = rand() % pokeList.size() + 1;

        ePkmn.push_back(new pokemon_entity(dist, pID));
    }

    drawTrainerFightMenu(player, ePkmn[eCurrPkmn], pCurrpkmn);

    refresh();

    int ch;
    bool fightOn = true;

    while (fightOn) {

        bool playerTurn = true;

        while (playerTurn) {
            pDmg = 0;
            eDmg = 0;
            pPrio = 6; // so that the player gets max prio when not fighting
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
                    // Clear the menu
                    for (int i = 15; i < 18; i++) {
                        for (int j = 17; j < 50; j++) {
                            mvaddch(i, j, ' ');
                        }
                    }

                    // print the fight menu
                    for (int i = 0; i < (int) player->pkmns[pCurrpkmn]->currMoves.size(); i++) {
                        mvprintw(14 + i, 19, (player->pkmns[pCurrpkmn]->currMoves[i]->identifier + " " +
                                              std::to_string(player->pkmns[pCurrpkmn]->PPList[i]) + "/" +
                                              std::to_string(player->pkmns[pCurrpkmn]->currMoves[i]->pp)).c_str());
                    }

                    int fightCurs = 0;

                    mvaddch(14 + fightCurs, 17, '>');

                    bool moved = false;

                    while (!moved) {
                        int ch4 = getch();

                        switch (ch4) {
                            case ' ':
                                if (player->pkmns[pCurrpkmn]->PPList[fightCurs] <= 0) {
                                    break; // don't let players use moves w/o PP
                                } else {
                                    player->pkmns[pCurrpkmn]->PPList[fightCurs]--; // if we have PP to spare, decrement it to use it
                                }

                                int crit, stab;
                                crit = 1;
                                stab = 1;

                                // See if this is a crit or not
                                if (rand() % 255 < (player->pkmns[pCurrpkmn]->pkmnBaseStats[6] / 2)) {
                                    crit = 1.5;
                                }

                                for (int i = 0; i < (int) player->pkmns[pCurrpkmn]->pkmnTypeList.size(); i++) {
                                    if (player->pkmns[pCurrpkmn]->pkmnTypeList[i]->type_id ==
                                        player->pkmns[pCurrpkmn]->currMoves[fightCurs]->type_id) {
                                        stab = 1.5;
                                    }
                                }

                                if (rand() % 100 < player->pkmns[pCurrpkmn]->currMoves[fightCurs]->accuracy ||
                                    player->pkmns[pCurrpkmn]->currMoves[fightCurs]->accuracy == -1) {
                                    if (player->pkmns[pCurrpkmn]->currMoves[fightCurs]->power != -1) {
                                        pDmg = ((((2 * player->pkmns[pCurrpkmn]->level) + 2) *
                                                 player->pkmns[pCurrpkmn]->currMoves[fightCurs]->power *
                                                 (player->pkmns[pCurrpkmn]->pkmnStats[1] /
                                                  player->pkmns[pCurrpkmn]->pkmnStats[2]) / 50) + 2) * crit *
                                               ((double) (rand() % 15 + 85) / 100) * stab;
                                    } else {
                                        pDmg = 0;
                                    }

                                    pPrio = player->pkmns[pCurrpkmn]->currMoves[fightCurs]->priority;
                                }

                                moved = true;
                                playerTurn = false;
                                break;

                            case KEY_DOWN:
                                if (fightCurs < (int) player->pkmns[pCurrpkmn]->currMoves.size() - 1) {
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

                    drawTrainerFightMenu(player, ePkmn[eCurrPkmn], pCurrpkmn);
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
                        mvprintw(i + 12, 20, (player->pkmns[i]->pkm->identifier + " HP: " +
                                              std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
                    }
                    mvprintw(i + 12, 20, "Back");

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
                                    playerTurn = false;
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

                    drawTrainerFightMenu(player, ePkmn[eCurrPkmn], pCurrpkmn);
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
                        mvprintw(i + 12, 40, (player->pkmns[i]->pkm->identifier + " HP: " +
                                              std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
                    }

                    mvprintw(i + 12, 40, "Back");

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
                                        mvprintw(22, 0, "You can't steal another trainer's pokemon!");
                                        break;

                                    case 1: // Potions
                                        if (player->potions > 0) {
                                            // Swap over to the pokemon list to choose who gets the potion
                                            mvaddch(12 + bagCursor, bagX, ' ');

                                            bagCursor = 0;
                                            bagX = 38;

                                            mvaddch(12 + bagCursor, bagX, '>');

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
                                                        } else if (player->pkmns[bagCursor]->pkmnStats[0] > 0 &&
                                                                   player->pkmns[bagCursor]->pkmnStats[0] <
                                                                   setPkmnHP(player->pkmns[bagCursor]->IVs[0],
                                                                             player->pkmns[bagCursor]->pkmnBaseStats[0],
                                                                             player->pkmns[bagCursor]->level)) {
                                                            // Give the selected pokemon either it's max hp or 20 more than it has right now
                                                            player->pkmns[bagCursor]->pkmnStats[0] = std::min(
                                                                    player->pkmns[bagCursor]->pkmnStats[0] += 20,
                                                                    setPkmnHP(player->pkmns[bagCursor]->IVs[0],
                                                                              player->pkmns[bagCursor]->pkmnBaseStats[0],
                                                                              player->pkmns[bagCursor]->level));
                                                            player->potions--;

                                                            bagCursor = 0;
                                                            bagX = 18;

                                                            potioned = true;
                                                            done = true;
                                                            playerTurn = false;
                                                        }
                                                        break;
                                                }
                                            }
                                        }
                                        break;

                                    case 2: // Revives
                                        if (player->revives > 0) {
                                            // Swap over to the pokemon list to choose who gets the potion
                                            mvaddch(12 + bagCursor, bagX, ' ');

                                            bagCursor = 0;
                                            bagX = 38;

                                            mvaddch(12 + bagCursor, bagX, '>');

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
                                                            player->pkmns[bagCursor]->pkmnStats[0] =
                                                                    (int) setPkmnHP(player->pkmns[bagCursor]->IVs[0],
                                                                                    player->pkmns[bagCursor]->pkmnBaseStats[0],
                                                                                    player->pkmns[bagCursor]->level) /
                                                                    2;
                                                            player->revives--;

                                                            bagCursor = 0;
                                                            bagX = 18;

                                                            revived = true;
                                                            done = true;
                                                            playerTurn = false;
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
                    drawTrainerFightMenu(player, ePkmn[eCurrPkmn], pCurrpkmn);
                    cursX = 17;
                    cursY = 15;
                } else if (cursX == 37 && cursY == 17) { // Run
                    fleeAttempts++;

                    escapeOdds = floor(((playerSpeed * 32) / ((int) (floor(ePkmn[eCurrPkmn]->pkmnStats[5] / 4)) % 256))) +
                                 30 * fleeAttempts;

                    if (escapeOdds > 255 || ((int) (ePkmn[eCurrPkmn]->pkmnStats[5] / 4)) % 256 == 0) {
                        for (int i = 0; i < (int) ePkmn.size(); i++) { // delete the trainer's pkmn
                            delete ePkmn[i];
                        }

                        fightOn = false;
                        playerTurn = false;
                        entity->inHeap = false;
                        refreshBuffer(world);
                        mvprintw(22, 0, "                                                  ");
                        return 0;
                    } else if (rand() % 256 < escapeOdds) {
                        for (int i = 0; i < (int) ePkmn.size(); i++) { // delete the trainer's pkmn
                            delete ePkmn[i];
                        }

                        fightOn = false;
                        playerTurn = false;
                        entity->inHeap = false;
                        refreshBuffer(world);
                        mvprintw(22, 0, "                                                  ");
                        return 0;
                    }

                    mvprintw(22, 0, "                                                  ");
                    playerTurn = false;
                }
            }
        }

        /// Wild pokemon "AI"

        if (fightOn) {
            int move = rand() % ePkmn[eCurrPkmn]->currMoves.size(); // Choose a random move from the list of moves
            int eCrit = 1, eStab = 1;

            // See if this is a crit or not
            if (rand() % 255 < (ePkmn[eCurrPkmn]->pkmnBaseStats[6] / 2)) {
                eCrit = 1.5;
            }

            // If we attack w/ a matching move update it to 1.5
            for (int i = 0; i < (int) ePkmn[eCurrPkmn]->pkmnTypeList.size(); i++) {
                if (ePkmn[eCurrPkmn]->pkmnTypeList[i]->type_id == ePkmn[eCurrPkmn]->currMoves[move]->type_id) {
                    eStab = 1.5;
                    break;
                }
            }

            if (rand() % 100 < ePkmn[eCurrPkmn]->currMoves[move]->accuracy || ePkmn[eCurrPkmn]->currMoves[move]->accuracy == -1) {
                if (ePkmn[eCurrPkmn]->currMoves[move]->power != -1) {
                    eDmg = ((((2 * ePkmn[eCurrPkmn]->level) + 2) * ePkmn[eCurrPkmn]->currMoves[move]->power * (ePkmn[eCurrPkmn]->pkmnStats[1] / ePkmn[eCurrPkmn]->pkmnStats[2]) / 50) + 2) * eCrit * ((double) (rand() % 15 + 85) / 100) * eStab;
                    mvprintw(22, 0, "%s uses %s for %d damage!", ePkmn[eCurrPkmn]->pkm->identifier.c_str(), ePkmn[eCurrPkmn]->currMoves[move]->identifier.c_str(), eDmg);
                } else {
                    eDmg = 0;
                    mvprintw(22, 0, "%s uses %s, but misses!", ePkmn[eCurrPkmn]->pkm->identifier.c_str(), ePkmn[eCurrPkmn]->currMoves[move]->identifier.c_str());
                }

                ePrio = ePkmn[eCurrPkmn]->currMoves[move]->priority;
            }

            if (ePrio > pPrio) { // Greater goes first, so then the enemy hits first
                player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);

                // If the player's pokemon lived through it, hit back
                if (player->pkmns[pCurrpkmn]->pkmnStats[0] != 0) {
                    ePkmn[eCurrPkmn]->pkmnStats[0] = std::max(0, ePkmn[eCurrPkmn]->pkmnStats[0] - pDmg);
                }
            } else if (pPrio > ePrio) { // Player has higher prio
                ePkmn[eCurrPkmn]->pkmnStats[0] = std::max(0, ePkmn[eCurrPkmn]->pkmnStats[0] - pDmg);

                // If the wild pokemon didn't die from that, then it can attack us
                if (ePkmn[eCurrPkmn]->pkmnStats[0] != 0) {
                    player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);
                }
            } else { // The same??? Idk random chance ffs why is this game so complicated
                int who = rand() % 2;

                if (who == 1) { // let the wild pokemon go first
                    player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);

                    // If the player's pokemon lived through it, hit back
                    if (player->pkmns[pCurrpkmn]->pkmnStats[0] != 0) {
                        ePkmn[eCurrPkmn]->pkmnStats[0] = std::max(0, ePkmn[eCurrPkmn]->pkmnStats[0] - pDmg);
                    }
                } else { // we go first
                    ePkmn[eCurrPkmn]->pkmnStats[0] = std::max(0, ePkmn[eCurrPkmn]->pkmnStats[0] - pDmg);

                    // If the wild pokemon didn't die from that, then it can attack us
                    if (ePkmn[eCurrPkmn]->pkmnStats[0] != 0) {
                        player->pkmns[pCurrpkmn]->pkmnStats[0] = std::max(player->pkmns[pCurrpkmn]->pkmnStats[0] - eDmg, 0);
                    }
                }
            }

            drawTrainerFightMenu(player, ePkmn[eCurrPkmn], pCurrpkmn);
        }

        if (player->pkmns[pCurrpkmn]->pkmnStats[0] <= 0) {
            // Clear the menu
            for (int i = 15; i < 18; i++) {
                for (int j = 17; j < 50; j++) {
                    mvaddch(i, j, ' ');
                }
            }

            // Check for a valid pokemon
            bool hasMorePokemon = false;

            for (int i = 0; i < player->pkmnCnt; i++) {
                if (player->pkmns[i]->pkmnStats[0] > 0) {
                    hasMorePokemon = true;
                    break;
                }
            }

            if (!hasMorePokemon) {
                for (int i = 0; i < (int) ePkmn.size(); i++) {
                    delete ePkmn[i];
                }
                entity->inHeap = false;
                mvprintw(22, 0, "                                                  ");
                return -1;
            }

            int faintCurs = 0;

            // Print out their pokemon
            for (int i = 0; i < player->pkmnCnt; i++) {
                mvprintw(12 + i, 20, (player->pkmns[i]->pkm->identifier + " HP: " + std::to_string(player->pkmns[i]->pkmnStats[0])).c_str());
            }

            mvaddch(12 + faintCurs, 18, '>');
            bool newPkmn = false;

            while (!newPkmn) {
                int ch5 = getch();

                switch (ch5) {
                    case ' ':
                        if (player->pkmns[faintCurs]->pkmnStats[0] != 0) {
                            pCurrpkmn = faintCurs;
                            newPkmn = true;
                            drawTrainerFightMenu(player, ePkmn[eCurrPkmn], pCurrpkmn);
                        }

                        break;

                    case KEY_DOWN:
                        if (faintCurs < player->pkmnCnt - 1) {
                            mvaddch(12 + faintCurs, 18, ' ');

                            faintCurs++;

                            mvaddch(12 + faintCurs, 18, '>');
                        }

                        break;

                    case KEY_UP:
                        if (faintCurs > 0) {
                            mvaddch(12 + faintCurs, 18, ' ');

                            faintCurs--;

                            mvaddch(12 + faintCurs, 18, '>');
                        }

                        break;
                }
            }
        }

        // If the trainer's pokemon is dead
        if (ePkmn[eCurrPkmn]->pkmnStats[0] <= 0) {
            // Grab the XP gain for the player
            xp += (ePkmn[eCurrPkmn]->pkm->base_experience * ePkmn[eCurrPkmn]->level) / (7 * player->pkmnCnt);

            // Check for a living pokemon
            bool more = false;

            for (int i = 0; i < (int) ePkmn.size(); i++) {
                if (ePkmn[i]->pkmnStats[0] > 0) {
                    eCurrPkmn = i;
                    more = true;
                    drawTrainerFightMenu(player, ePkmn[eCurrPkmn], pCurrpkmn);
                    break;
                }
            }

            // If there are no more pokemon to choose from, then we win, and the trainer goes away
            if (!more) {
                entity->inHeap = false;
                refreshBuffer(world);
                mvprintw(22, 0, "                                                  ");
                fightOn = false;
            }
        }
    }

    // Calculate XP earned (for simplicity it will be only to current Pokemon)
    // Crude implementation of formula from https://bulbapedia.bulbagarden.net/wiki/Experience#Experience_gain_in_battle
    xp += (ePkmn[eCurrPkmn]->pkm->base_experience * ePkmn[eCurrPkmn]->level) / (7 * player->pkmnCnt);

    for (int i = 0; i < player->pkmnCnt; i++) {
        player->pkmns[i]->xp += xp;
        player->pkmns[i]->levelup(); // try to levelup each pokemon
    }

    for (int i = 0; i < (int) ePkmn.size(); i++) {
        delete ePkmn[i];
    }

    // Now that that hell is over, we should probably leave the function now
    entity->inHeap = false;
    refreshBuffer(world);
    mvprintw(22, 0, "                                                  ");
    return 0;
}