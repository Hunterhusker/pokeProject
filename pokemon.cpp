//
// Created by hunter on 3/30/22.
//

#include "pokemon.h"

pokemon::pokemon(int id, std::string identifier, int species_id, int height, int weight, int base_experience, int order, int isDefault) {
    this->id = id;
    this->identifier = identifier;
    this->species_id = species_id;
    this->height = height;
    this->weight = weight;
    this->base_experience = base_experience;
    this->order = order;
    this->isDefault = isDefault;
}

std::ostream &operator<< (std::ostream &o, pokemon p) {
    return o << p.identifier << " ID:" << p.id << " SpeciesID: " << p.species_id << " Height: " << p.height << " Weight: " << p.weight << " BaseExp: " << p.base_experience << " Order: " << p.order << " Default: " << p.isDefault;
}
