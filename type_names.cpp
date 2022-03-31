//
// Created by hunter on 3/31/22.
//

#include "type_names.h"

type_names::type_names(int type_id, int local_language_id, std::string name) {
    this->type_id = type_id;
    this->local_language_id = local_language_id;
    this->name = name;
}

std::ostream &operator<< (std::ostream &o, type_names t) {
    return o << "Type: " << t.name << " typeID: " << t.type_id << " languageID: " << t.local_language_id;
}