#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "heatMap.h"
#include "minHeap.h"
#include "mapBuilder.h"

/**
 * This method fills up a heat map of sorts with distances from the player
 *
 * @param screen ~ the screen that the player is currently on
 * @param heatMap ~ heatMap struct to fill with the info
 * @param player ~ the player so we can find it's location and deal with the edge cases
 * @param enemyType ~ the type of enemy we have, so we can have custom weights for custom enemies
 */
void fillHeatMap(map_t *screen, heatMap_t *heatMap, cell_t *player, char enemyType)
{
    minHeap_t mh;
    mh.currLen = 0;

    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            heatMap->heatMap[i][j].dist = INT_MAX;
            heatMap->heatMap[i][j].inHeap = false;
            heatMap->heatMap[i][j].x = j;
            heatMap->heatMap[i][j].y = i;

            if (screen->map[i][j].type != '%' && screen->map[i][j].type != 'C' && screen->map[i][j].type != 'M') {
                heatMap->heatMap[i][j].inHeap = true;
                mhAdd(&mh, &heatMap->heatMap[i][j]);
            }
        }
    }

    heatMap->heatMap[player->y][player->x].dist = 0;

    heapifyUpCell(&mh, &screen->map[player->y][player->x]);

    while (mh.currLen > 0) {
        cell_t u = mhExtract(&mh);
        u.inHeap = false;

        if (u.x - 1 >= 0 && heatMap->heatMap[u.y][u.x - 1].inHeap == true && heatMap->heatMap[u.y][u.x - 1].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            heatMap->heatMap[u.y][u.x - 1].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &heatMap->heatMap[u.y][u.x - 1]);

            if (u.y - 1 >= 0 && heatMap->heatMap[u.y - 1][u.x - 1].inHeap == true && heatMap->heatMap[u.y - 1][u.x - 1].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                heatMap->heatMap[u.y - 1][u.x - 1].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &heatMap->heatMap[u.y - 1][u.x - 1]);
            }

            if (u.y + 1 < 21 && heatMap->heatMap[u.y + 1][u.x - 1].inHeap == true && heatMap->heatMap[u.y + 1][u.x - 1].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                heatMap->heatMap[u.y + 1][u.x - 1].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &heatMap->heatMap[u.y + 1][u.x - 1]);
            }
        }

        if (u.x + 1 < 80 && heatMap->heatMap[u.y][u.x + 1].inHeap == true && heatMap->heatMap[u.y][u.x + 1].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            heatMap->heatMap[u.y][u.x + 1].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &heatMap->heatMap[u.y][u.x + 1]);

            if (u.y - 1 >= 0 && heatMap->heatMap[u.y - 1][u.x + 1].inHeap == true && heatMap->heatMap[u.y - 1][u.x + 1].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                heatMap->heatMap[u.y - 1][u.x + 1].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &heatMap->heatMap[u.y - 1][u.x + 1]);
            }

            if (u.y + 1 < 21 && heatMap->heatMap[u.y + 1][u.x + 1].inHeap == true && heatMap->heatMap[u.y + 1][u.x + 1].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                heatMap->heatMap[u.y + 1][u.x + 1].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &heatMap->heatMap[u.y + 1][u.x + 1]);
            }
        }

        if (u.y - 1 >= 0 && heatMap->heatMap[u.y - 1][u.x].inHeap == true && heatMap->heatMap[u.y - 1][u.x].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            heatMap->heatMap[u.y - 1][u.x].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &heatMap->heatMap[u.y - 1][u.x]);
        }

        if (u.y + 1 < 21 && heatMap->heatMap[u.y + 1][u.x].inHeap == true && heatMap->heatMap[u.y + 1][u.x].dist > heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            heatMap->heatMap[u.y + 1][u.x].dist = heatMap->heatMap[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &heatMap->heatMap[u.y + 1][u.x]);
        }
    }
}

/**
 * Returns the cost associated with a given biome type
 *
 * @param biomeType ~ the char representing what type of land it is
 * @param npcType ~ the type of npc that is asking about the cost
 * @param player ~ the player for checking the edge case
 * @return ~ the cost of moving to that location
 */
int determineCost(char biomeType, char npcType)
{
    switch (npcType) {
        case 'h':
            switch (biomeType) {
                case '#':
                case '.':
                    return 10;

                case ':':
                    return 15;

                case 'M':
                case 'C':
                case '%':
                    return INT_MAX; // -1 is infinity for me because lazy

                default:
                    return 0; // 0 is an uh oh for this function since anything should have a cost
            }

        // Rival costs are the same as the others, so I added them here for cost calculation
        case 'r':
        case 'p':
        case 's':
        case 'w':
        case 'n':
            switch (biomeType) {
                case '#':
                case '.':
                    return 10;

                case ':':
                    return 20;

                case 'M':
                case 'C':
                case '%':
                    return INT_MAX; // -1 is infinity for me because lazy

                default:
                    return 0; // 0 is an uh oh for this function since anything should have a cost
            }

        // Case for calculating the costs of movement for the player
        case '@':
            switch (biomeType) {
                case '#':
                case '.':
                    return 10;

                case ':':
                    return 20;

                case '%':
                case 'M': //TODO PUT THESE BACK ON 10 WHEN WE CAN USE THEM
                case 'C':
                    return INT_MAX; // -1 is infinity for me because lazy

                default:
                    return 0; // 0 is an uh oh for this function since anything should have a cost
            }

        default:
            return INT_MAX;
    }
}

/**
 * Takes the given heat map and then prints it out to the console, also skipping infinities with two spaces, and
 *
 * @param heatMap ~ the heat map struct that holds all the information for the heatmap
 */
void printHeatMap(heatMap_t *heatMap)
{
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (heatMap->heatMap[i][j].dist == INT_MAX) {
                printf("   ");
            } else {
                printf("%02d ", heatMap->heatMap[i][j].dist % 100);
            }
        }
        printf("\n");
    }
}