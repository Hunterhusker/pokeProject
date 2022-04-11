//
// Created by hunter on 4/10/22.
//
#include "minHeap.cpp"
#include <iostream>
#include "pokemon.h"
#include "fileReader.h"

class cell {
public:
    char type;
    int weight;
    int instance;
    int parentLoc[2];
    int dist;
    int x;
    int y;
    bool inHeap;
};

class entity_cell : public cell {
public:
    pokemon_entity *pkmns[6];
    ~entity_cell();
    entity_cell() {}
    entity_cell(int x, int y, char type, int weight, int dist);
};

entity_cell::~entity_cell() {
    for (int i = 0; i < 6; i++) {
        if (this->pkmns[i] != NULL) {
            delete this->pkmns[i];
        }
    }
}

entity_cell::entity_cell(int x, int y, char type, int weight, int dist) {
    this->x = x;
    this->y = y;
    this->type = type;
    this->weight = weight;
    this->dist = dist;

    for (int i = 0; i < 6; i++) {
        this->pkmns[i] = NULL;
    }
}

int main() {
    minHeap<entity_cell> mh;

    entity_cell ec1(1, 1, 'A', 10, 5);
    entity_cell ec2(1, 1, 'b', 10, 10);
    entity_cell ec3(1, 1, 'c', 10, 15);
    entity_cell ec4(1, 1, 'd', 10, 20);
    entity_cell ec5(1, 1, 'e', 10, 25);
    entity_cell ec6(1, 1, 'f', 10, 2);

    mh.add(&ec1, &ec2.dist);
    mh.add(&ec2, &ec5.dist);
    mh.add(&ec3, &ec4.dist);
    mh.add(&ec4, &ec3.dist);
    mh.add(&ec5, &ec1.dist);
    mh.add(&ec6, &ec6.dist);

    //std::cout << mh << std::endl;

    while (!mh.isEmpty()) {
        std::cout << mh.extract()->dist << std::endl;
    }
}
