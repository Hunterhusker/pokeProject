//
// Created by hunter on 4/16/22.
//

#ifndef POKEPROJECT_WORLD_H
#define POKEPROJECT_WORLD_H

class map;

class world {
public:
    map *board[399][399];
    int currX;
    int currY;
    int entityCount;
};

void gameBoardInit(world *world);
void printCurr(world *world);
void worldInit(world *world);
void destroyWorld(world *world);
int isScreenGenerated(world *world, int x, int y);
int buildingChance(int x, int y);
int goToLoc(world *world, int x, int y);

#endif //POKEPROJECT_WORLD_H
