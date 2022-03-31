//
// Created by hunter on 3/31/22.
//

#ifndef POKEPROJECT_POKEMON_MOVES_H
#define POKEPROJECT_POKEMON_MOVES_H

#include <string>
#include <iostream>

class pokemon_moves {
public:
    int pokemon_id, version_group_id, move_id, pokemon_move_method_id, level, order;

    pokemon_moves(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order);
};

std::ostream &operator<< (std::ostream &o, pokemon_moves pm);

#endif //POKEPROJECT_POKEMON_MOVES_H
