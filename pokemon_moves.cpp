//
// Created by hunter on 3/31/22.
//

#include "pokemon_moves.h"

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