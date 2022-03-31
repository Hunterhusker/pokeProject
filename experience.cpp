//
// Created by hunter on 3/31/22.
//

#include "experience.h"

experience::experience(int growth_rate_id, int level, int experienceAmt) {
    this->growth_rate_id = growth_rate_id;
    this->level = level;
    this->experienceAmt = experienceAmt;
}

std::ostream &operator<< (std::ostream &o, experience e) {
    return o << "Growth Rate: " << e.growth_rate_id << " Level: " << e.level << " Experience: " << e.experienceAmt;
}