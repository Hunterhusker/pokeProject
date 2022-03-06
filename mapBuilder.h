#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <stdbool.h>

/// The struct to represent each individual possible entity location, and store the entities seperate from the maps
//typedef struct entity {
//    char type;
//    int x, y;
//} entity_t;

// The map struct to represent each individual spot on the map
typedef struct cell
{
    char type;
    int weight;
    int instance;
    int parentLoc[2];
    int dist;
    int x;
    int y;
    bool inHeap;
} cell_t;

typedef struct map
{
    cell_t map[21][80];
    cell_t *eMap[21][80];
    int exits[4][2];
} map_t;

/// Heap structs so that they can be used in here
typedef struct heapNode
{
    cell_t  *data;
    //void *data;
    int *value;
    int x, y;
} heapNode_t;

typedef struct minHeap
{
    heapNode_t heap[1860];
    int currLen;
} minHeap_t;

// Map Methods
void printMap(map_t *screen);
void emptyEntityMap(cell_t *eMap[21][80]);
void deleteAllEntities(cell_t *eMap[21][80]);
void emptyGrid(cell_t map[21][80]);
void placeExits(cell_t map[21][80], int exits[4][2], int e1, int e2, int e3, int e4);
int placeBiomes(cell_t map[21][80], int filledTiles[][2]);
void copyCell(cell_t *a, cell_t *b);
void grow(cell_t map[21][80], int filledTiles[1482][2], int curr);
void findNeighbors(cell_t map[21][80], cell_t *cell, int neighbors[4][2]);
void dijkstra(int x1, int y1, int x2, int y2, cell_t map[21][80]);
void layPath(cell_t map[21][80], int exits[4][2]);
void addShop(cell_t map[21][80], char shopIcon);
void generate(int e1, int e2, int e3, int e4, map_t *board, int shopChance);

// Player Methods
void placePlayer(map_t *map, cell_t *player, minHeap_t *mh);
void unplacePlayer(map_t *map, cell_t *player, minHeap_t *mh);
void movePlayer(map_t *screen, cell_t *player, minHeap_t *mh);

// Entity methods
cell_t* placeEntity(map_t *screen, minHeap_t *mh, char type);
int delEntity(map_t *screen, minHeap_t *mh, cell_t *entity);
int moveEntity(map_t *screen, minHeap_t *mh, cell_t *entity);

//// Rival Methods
//cell_t placeRival(map_t *screen, minHeap_t *mh);
//int delRival(map_t *screen, cell_t *rival, minHeap_t *mh);
//int nextRivalMove(map_t *screen, cell_t *rival, minHeap_t *mh);
//
//// Hiker methods
//cell_t placeHiker(map_t *screen, minHeap_t *mh);
//int delHiker(map_t *screen, cell_t *hiker, minHeap_t *mh);
//int nextHikerMove(map_t *screen, cell_t *hiker, minHeap_t *mh);
//
//// Stationary Enemy methods
//cell_t placeStationary(map_t *screen, minHeap_t *mh);

#endif
