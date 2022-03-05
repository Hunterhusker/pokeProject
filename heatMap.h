#ifndef HEATMAP_H
#define HEATMAP_H

#include "mapBuilder.h"

typedef struct heatMap {
    int *heatMap[21][80];
} heatMap_t;

void fillHeatMap(map_t *map, heatMap_t *heatMap, cell_t *player, char enemyType);
int determineCost(char biomeType, char npcType);
void printHeatMap(heatMap_t *heatMap);

#endif