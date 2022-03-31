//
// Created by hunter on 3/31/22.
//

#ifndef POKEPROJECT_EXPERIENCE_H
#define POKEPROJECT_EXPERIENCE_H

#include <iostream>
#include <string>

class experience {
public:
    int growth_rate_id, level, experienceAmt;

    experience(int growth_rate_id, int level, int experienceAmt);
};

std::ostream &operator<< (std::ostream &o, experience e);

#endif //POKEPROJECT_EXPERIENCE_H
