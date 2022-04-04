#include "pokemon.h"

/// Pokemon Class stuff
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


/// Experience class stuff
experience::experience(int growth_rate_id, int level, int experienceAmt) {
    this->growth_rate_id = growth_rate_id;
    this->level = level;
    this->experienceAmt = experienceAmt;
}

std::ostream &operator<< (std::ostream &o, experience e) {
    return o << "Growth Rate: " << e.growth_rate_id << " Level: " << e.level << " Experience: " << e.experienceAmt;
}

/// Moves class stuff
moves::moves(int id, std::string identifier, int generation_id, int type_id, int power, int pp, int accuracy,
             int priority, int target_id, int damage_class_id, int effect_id, int effect_chance, int contest_type_id,
             int contest_effect_id, int super_contest_effect_id) {
    this->id = id;
    this->identifier = identifier;
    this->generation_id = generation_id;
    this->type_id = type_id;
    this->power = power;
    this->pp = pp;
    this->accuracy = accuracy;
    this->priority = priority;
    this->target_id = target_id;
    this->damage_class_id = damage_class_id;
    this->effect_id = effect_id;
    this->effect_chance = effect_chance;
    this->contest_type_id = contest_type_id;
    this->contest_effect_id = contest_effect_id;
    this->super_contest_effect_id = super_contest_effect_id;
}

std::ostream &operator<< (std::ostream &o, moves m) {
    return o << " Id: " << m.id << " Name: " << m.identifier << " GenID: " << m.generation_id << " TypeID: " << m.type_id <<
             " Power: " << m.power << " PP: " << m.pp << " Accuracy: " << m.accuracy << " Priority: " << m.priority << " TargetID: " <<
             m.target_id << " DamageClassID: " << m.damage_class_id << " EffectID: " << m.effect_id << " EffectChance: " << m.effect_chance <<
             " Contest Type ID: " << m.contest_type_id << " Contest Effect ID: " << m.contest_effect_id << " Super Contest Effect ID: " << m.super_contest_effect_id;
}

/// PokeMoves Class stuff
pokemon_moves::pokemon_moves(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level,
                             int order) {
    this->pokemon_id = pokemon_id;
    this->version_group_id = version_group_id;
    this->move_id = move_id;
    this->pokemon_move_method_id = pokemon_move_method_id;
    this->level = level;
    this->order = order;
}

std::ostream &operator<< (std::ostream &o, pokemon_moves pm) {
    return o << "PokemonID: " << pm.pokemon_id << " Version GroupID: " << pm.version_group_id << " MoveID: " << pm.move_id <<
             " Pokemon Move Method: " << pm.pokemon_move_method_id << " Level: " << pm.level << " Order: " << pm.order;
}

/// Poke Species Class Stuff
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

/// Type names class stuff
type_names::type_names(int type_id, int local_language_id, std::string name) {
    this->type_id = type_id;
    this->local_language_id = local_language_id;
    this->name = name;
}

// Pokemon_stats parsing
pokemon_stats::pokemon_stats(int pokemon_id, int stat_id, int base_stat, int effort) {
    this->pokemon_id = pokemon_id;
    this->base_stat = base_stat;
    this->effort = effort;
    this->stat_id = stat_id;
}

// Stats parsing
stats::stats(int id, int damage_class_id, std::string identifier, int is_battle_only, int game_index) {
    this->id = id;
    this->damage_class_id = damage_class_id;
    this->identifier = identifier;
    this->is_battle_only = is_battle_only;
    this->game_index = game_index;
}

std::ostream &operator<< (std::ostream &o, type_names t) {
    return o << "Type: " << t.name << " typeID: " << t.type_id << " languageID: " << t.local_language_id;
}