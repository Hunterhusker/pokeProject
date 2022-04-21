#ifndef HEATMAP_H
#define HEATMAP_H

#include "mapBuilder.h"

class heatMap {
public:
    cell heatMap[21][80];
};

void fillHeatMap(map *map, heatMap *heatMap, player_cell *player, char enemyType);
int determineCost(char biomeType, char npcType);
void printHeatMap(heatMap *heatMap);

#endif