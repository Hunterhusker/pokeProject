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

int readAllPokemon(std::vector<pokemon> &list);
int readAllPokemonSpecies(std::vector<pokemon_species> &list);
int readAllExperience(std::vector<experience> &list);
int readAllTypes(std::vector<type_names> &list);

#endif //POKEPROJECT_FILEREADER_H
