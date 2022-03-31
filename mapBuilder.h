#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include "minHeap.h"

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

class map {
public:
    cell map[21][80];
    cell *eMap[21][80];
    int exits[4][2];
    minHeap mh;
};

// Map Methods
void printMap(map *screen);
void emptyEntityMap(cell *eMap[21][80]);
void deleteAllEntities(cell *eMap[21][80]);
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
cell* placeEntity(map *screen, minHeap *mh, char type);
void delEntity(map *screen, minHeap *mh, cell *entity);
int moveEntity(map *screen, minHeap *mh, cell *entity, cell *player);
int fightPLayer(map *screen, cell *entity, cell *player);
void printAmogus();

#endif
