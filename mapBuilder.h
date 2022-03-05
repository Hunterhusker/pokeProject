#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <stdbool.h>

//// The struct to represent each individual possible entity location, and store the entities seperate from the maps
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

// Map Methods
void printMap(map_t *screen);
void emptyEntityMap(cell_t *eMap[21][80]);
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
void placePlayer(map_t *map, cell_t *player);
void unplacePlayer(map_t *map, cell_t *player);

#endif
