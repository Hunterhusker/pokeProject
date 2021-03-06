//
// Created by hunter on 3/30/22.
//

#ifndef POKEPROJECT_FILEREADER_H
#define POKEPROJECT_FILEREADER_H

#include <fstream>
#include <vector>

class pokemon;
class pokemon_species;
class experience;
class type_names;
class pokemon_moves;
class moves;
class stats;
class pokemon_stats;
class pokemon_types;

int readAllPokemon(std::vector<pokemon> &list);
int readAllPokemonSpecies(std::vector<pokemon_species> &list);
int readAllExperience(std::vector<experience> &list);
int readAllTypes(std::vector<type_names> &list);
int readAllPokemonMoves(std::vector<pokemon_moves> &list);
int readAllMoves(std::vector<moves> &list);
int readAllStats(std::vector<stats> &list);
int readAllPokemonStats(std::vector<pokemon_stats> &list);
int readAllPokemonTypes(std::vector<pokemon_types> &list);

#endif //POKEPROJECT_FILEREADER_H
