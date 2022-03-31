//
// Created by hunter on 3/31/22.
//

#ifndef POKEPROJECT_POKEMON_SPECIES_H
#define POKEPROJECT_POKEMON_SPECIES_H

#include <iostream>
#include <string>

class pokemon_species {
public:
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

    pokemon_species(int id, std::string identifier, int generation_id, int evolves_from_species_id, int evolution_chain_id, int color_id, int shape_id, int habitat_id, int gender_rate, int capture_rate,
                    int base_happiness, int is_baby, int hatch_counter, int has_gender_differences, int growth_rate_id, int forms_switchable, int is_legendary, int is_mythical, int order, int conquest_order);
};

std::ostream &operator<< (std::ostream &o, pokemon_species p);

#endif //POKEPROJECT_POKEMON_SPECIES_H
