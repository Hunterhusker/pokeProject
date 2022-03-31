//
// Created by hunter on 3/31/22.
//

#include "pokemon_species.h"

pokemon_species::pokemon_species(int id, std::string identifier, int generation_id, int evolves_from_species_id,
                                 int evolution_chain_id, int color_id, int shape_id, int habitat_id, int gender_rate,
                                 int capture_rate, int base_happiness, int is_baby, int hatch_counter,
                                 int has_gender_differences, int growth_rate_id, int forms_switchable, int is_legendary,
                                 int is_mythical, int order, int conquest_order) {
    this->id = id;
    this->identifier = identifier;
    this->generation_id = generation_id;
    this->evolves_from_species_id = evolves_from_species_id;
    this->evolution_chain_id = evolution_chain_id;
    this->color_id = color_id;
    this->shape_id = shape_id;
    this->habitat_id = habitat_id;
    this->gender_rate = gender_rate;
    this->capture_rate = capture_rate;
    this->base_happiness = base_happiness;
    this->is_baby = is_baby;
    this->hatch_counter = hatch_counter;
    this->has_gender_differences = has_gender_differences;
    this->growth_rate_id = growth_rate_id;
    this->forms_switchable = forms_switchable;
    this->is_legendary = is_legendary;
    this->is_mythical = is_mythical;
    this->order = order;
    this->conquest_order = conquest_order;
}

std::ostream &operator<< (std::ostream &o, pokemon_species p) {
    return o << p.identifier << " ID: " << p.id << " GenID: " << p.generation_id << " Evolves from ID: " << p.evolves_from_species_id <<
    " Evolution Chain ID: " << p.evolution_chain_id << " ColorID: " << p.color_id << " ShapeID: " << p.shape_id << " HabitatID: " <<
    p.habitat_id << " GenderRate: " << p.gender_rate << " Capture Rate: " << p.capture_rate << " Base Happiness: " << p.base_happiness << // mine is 0 rn
    " Is Baby: " << p.is_baby << " HatchCounter: " << p.hatch_counter << " GenderDiffs: " << p.has_gender_differences << " GrowthRateID: " <<
    p.growth_rate_id << " Forms Switchable: " << p.forms_switchable << " Is Legendary: " << p.is_legendary << " Is Mythical: " <<
    p.is_mythical << " Order: " << p.order << " ConquestOrder: " << p.conquest_order;
}