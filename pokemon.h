//
// Created by hunter on 3/30/22.
//
#ifndef POKEPROJECT_POKEMON_H
#define POKEPROJECT_POKEMON_H

#include <iostream>
#include <string>
#include <vector>

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

class experience {
public:
    int growth_rate_id, level, experienceAmt;

    experience(int growth_rate_id, int level, int experienceAmt);
};

class moves {
public:
    int id, generation_id, type_id, power, pp, accuracy, priority, target_id, damage_class_id, effect_id, effect_chance, contest_type_id, contest_effect_id, super_contest_effect_id;
    std::string identifier;

    moves(int id, std::string identifier, int generation_id, int type_id, int power, int pp, int accuracy, int priority,
          int target_id, int damage_class_id, int effect_id, int effect_chance, int contest_type_id, int contest_effect_id,
          int super_contest_effect_id);
};

class pokemon_moves {
public:
    int pokemon_id, version_group_id, move_id, pokemon_move_method_id, level, order;

    pokemon_moves(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order);
};

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

    pokemon_species(int id, std::string identifier, int generation_id, int evolves_from_species_id, int evolution_chain_id,
                    int color_id, int shape_id, int habitat_id, int gender_rate, int capture_rate, int base_happiness,
                    int is_baby, int hatch_counter, int has_gender_differences, int growth_rate_id, int forms_switchable,
                    int is_legendary, int is_mythical, int order, int conquest_order);
};

class type_names {
public:
    int type_id, local_language_id;
    std::string name;

    type_names(int type_id, int local_language_id, std::string name);
};

class pokemon_stats {
public:
    int pokemon_id, stat_id, base_stat, effort;

    pokemon_stats(int pokemon_id, int stat_id, int base_stat, int effort);
};

class stats {
public:
    int id, damage_class_id, is_battle_only, game_index;
    std::string identifier;

    stats(int id, int damage_class_id, std::string identifier, int is_battle_only, int game_index);
};

class pokemon_entity {
public:
    pokemon *pkm;
    pokemon_species *species;
    std::vector<pokemon_moves *> moveSet;
    std::vector<moves *> currMoves;
    experience *exp;
    //type_names *type;
    int xp, level;
    std::vector<int> IVs, pkmnStats, pkmnBaseStats;
    bool shiny;
    bool male;

    pokemon_entity(std::vector<pokemon> &pokeList, std::vector<pokemon_species> &speciesList, std::vector<experience> &expList,
                   std::vector<type_names> &typeList, std::vector<pokemon_moves> &pkmMovesList, std::vector<moves> &mvList,
                   std::vector<pokemon_stats> &pkmStatList, std::vector<stats> &statList, int distance, int id);
};

std::ostream &operator<< (std::ostream &o, pokemon p);
std::ostream &operator<< (std::ostream &o, experience e);
std::ostream &operator<< (std::ostream &o, moves m);
std::ostream &operator<< (std::ostream &o, pokemon_moves pm);
std::ostream &operator<< (std::ostream &o, pokemon_species p);
std::ostream &operator<< (std::ostream &o, type_names t);
std::ostream &operator<< (std::ostream &o, pokemon_entity pe);

int setPkmnHP(int hpIV, int base, int level);
int setOtherPkmnStat(int IV, int base, int level);

#endif //POKEPROJECT_POKEMON_H
