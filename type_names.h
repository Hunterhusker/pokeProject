//
// Created by hunter on 3/31/22.
//

#ifndef POKEPROJECT_TYPE_NAMES_H
#define POKEPROJECT_TYPE_NAMES_H

#include <string>
#include <iostream>

class type_names {
public:
    int type_id, local_language_id;
    std::string name;

    type_names(int type_id, int local_language_id, std::string name);
};

std::ostream &operator<< (std::ostream &o, type_names t);

#endif //POKEPROJECT_TYPE_NAMES_H
