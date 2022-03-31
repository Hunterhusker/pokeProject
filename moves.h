//
// Created by hunter on 3/31/22.
//

#ifndef POKEPROJECT_MOVES_H
#define POKEPROJECT_MOVES_H

#include <ostream>
#include <string>

class moves {
public:
    int id, generation_id, type_id, power, pp, accuracy, priority, target_id, damage_class_id, effect_id, effect_chance, contest_type_id, contest_effect_id, super_contest_effect_id;
    std::string identifier;

    moves(int id, std::string identifier, int generation_id, int type_id, int power, int pp, int accuracy, int priority,
          int target_id, int damage_class_id, int effect_id, int effect_chance, int contest_type_id, int contest_effect_id,
          int super_contest_effect_id);
};

std::ostream &operator<< (std::ostream &o, moves m);

#endif //POKEPROJECT_MOVES_H
