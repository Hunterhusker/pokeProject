#ifndef POKEPROJECT_MENUS_H
#define POKEPROJECT_MENUS_H

#include "mapBuilder.h"
#include "world.h"
#include "global.h"

void fightRandomPokemon(int dist, player_cell *player);
int trainerMenu(world *world, player_cell *player);
int shopMenu(char shopType);
void bagMenu(player_cell *player);
void startScreen(player_cell *player);
void printAmogus();
int fightPLayer(world *world, entity_cell *entity, player_cell *player);

#endif //POKEPROJECT_MENUS_H
