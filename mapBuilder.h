#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <stdbool.h>

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

/// Heap structs so that they can be used in here
typedef struct heapNode
{
    cell_t  *data;
    int *value;
    //int x, y;
} heapNode_t;

typedef struct minHeap
{
    heapNode_t heap[1860];
    int currLen;
} minHeap_t;

typedef struct map
{
    cell_t map[21][80];
    cell_t *eMap[21][80];
    int exits[4][2];
    minHeap_t mh;
} map_t;

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

// Entity methods
cell_t* placeEntity(map_t *screen, minHeap_t *mh, char type);
void delEntity(map_t *screen, minHeap_t *mh, cell_t *entity);
int moveEntity(map_t *screen, minHeap_t *mh, cell_t *entity, cell_t *player);

#endif
