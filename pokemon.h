//
// Created by hunter on 3/30/22.
//
#ifndef POKEPROJECT_POKEMON_H
#define POKEPROJECT_POKEMON_H

#include <iostream>
#include <string>

class pokemon {
public:
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int isDefault;

    pokemon(int id, std::string identifier, int species_id, int height, int weight, int base_experience, int order, int isDefault);
};

std::ostream &operator<< (std::ostream &o, pokemon p);

#endif //POKEPROJECT_POKEMON_H
