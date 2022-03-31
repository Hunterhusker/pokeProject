//
// Created by hunter on 3/31/22.
//

#include "moves.h"

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