#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include "minHeap.h"
#include "pokemon.h"

class world;

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
    int pkmnCnt;

    ~entity_cell();
    entity_cell() {}
    entity_cell(int x, int y, char type, int weight, int dist);
};

class player_cell : public entity_cell {
public:
    int pokeballs;
    int revives;
    int potions;
    int balance;

    player_cell() {}
    player_cell(int x, int y, char type, int weight, int dist, int ballCnt, int reviveCnt, int potionCnt, int balance);
    ~player_cell();
};

class map {
public:
    cell map[21][80];
    entity_cell *eMap[21][80];
    int exits[4][2];
    minHeap mh;
};

// Map Methods
void emptyEntityMap(entity_cell *eMap[21][80]);
void deleteAllEntities(entity_cell *eMap[21][80]);
void emptyGrid(cell map[21][80]);
void placeExits(cell map[21][80], int exits[4][2], int e1, int e2, int e3, int e4);
int placeBiomes(cell map[21][80], int filledTiles[][2]);
void copyCell(cell *a, cell *b);
void grow(cell map[21][80], int filledTiles[1482][2], int curr);
void findNeighbors(cell map[21][80], cell *cell, int neighbors[4][2]);
void dijkstra(int x1, int y1, int x2, int y2, cell map[21][80]);
void layPath(cell map[21][80], int exits[4][2]);
void addShop(cell map[21][80], char shopIcon);
void generate(int e1, int e2, int e3, int e4, map *board, int shopChance, int entityCount);
int placeEntities(int entityCount, map *screen, minHeap *mh);

// Entity methods
entity_cell* placeEntity(map *screen, minHeap *mh, char type);
player_cell *placePlayer(map *screen, minHeap *mh);
void delEntity(map *screen, minHeap *mh, entity_cell *entity);
int moveEntity(world *world, map *screen, minHeap *mh, entity_cell *entity, player_cell *player);
int movePlayer(int y, int x, world *world, player_cell *player, std::string message);

#endif
