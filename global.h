#ifndef POKEPROJECT_GLOBAL_H
#define POKEPROJECT_GLOBAL_H

#include "pokemon.h"
#include <vector>

extern std::vector<pokemon> pokeList;
extern std::vector<pokemon_species> species;
extern std::vector<experience> expList;
extern std::vector<type_names> types;
extern std::vector<pokemon_moves> pkmnMoves;
extern std::vector<moves> mvList;
extern std::vector<pokemon_stats> pkStats;
extern std::vector<stats> statList;
extern std::vector<pokemon_types> pkmnTypes;

#endif //POKEPROJECT_GLOBAL_H
