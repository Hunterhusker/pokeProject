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
            screen->map[i][j].dist = INT_MAX;
            screen->map[i][j].inHeap = false;
            heatMap->heatMap[i][j] = &screen->map[i][j].dist;

            if (screen->map[i][j].type != '%' && screen->map[i][j].type != 'C' && screen->map[i][j].type != 'M') {
                screen->map[i][j].inHeap = true;
                mhAdd(&mh, &screen->map[i][j]);
            }
        }
    }

    screen->map[player->y][player->x].dist = 0;

    heapifyUpCell(&mh, &screen->map[player->y][player->x]);

    while (mh.currLen > 0) {
        cell_t u = mhExtract(&mh);
        u.inHeap = false;

        if (u.x - 1 >= 0 && screen->map[u.y][u.x - 1].inHeap == true && screen->map[u.y][u.x - 1].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            screen->map[u.y][u.x - 1].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &screen->map[u.y][u.x - 1]);

            if (u.y - 1 >= 0 && screen->map[u.y - 1][u.x - 1].inHeap == true && screen->map[u.y - 1][u.x - 1].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                screen->map[u.y - 1][u.x - 1].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &screen->map[u.y - 1][u.x - 1]);
            }

            if (u.y + 1 < 21 && screen->map[u.y + 1][u.x - 1].inHeap == true && screen->map[u.y + 1][u.x - 1].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                screen->map[u.y + 1][u.x - 1].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &screen->map[u.y + 1][u.x - 1]);
            }
        }

        if (u.x + 1 < 80 && screen->map[u.y][u.x + 1].inHeap == true && screen->map[u.y][u.x + 1].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            screen->map[u.y][u.x + 1].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &screen->map[u.y][u.x + 1]);

            if (u.y - 1 >= 0 && screen->map[u.y - 1][u.x + 1].inHeap == true && screen->map[u.y - 1][u.x + 1].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                screen->map[u.y - 1][u.x + 1].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &screen->map[u.y - 1][u.x + 1]);
            }

            if (u.y + 1 < 21 && screen->map[u.y + 1][u.x + 1].inHeap == true && screen->map[u.y + 1][u.x + 1].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
                screen->map[u.y + 1][u.x + 1].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

                heapifyUpCell(&mh, &screen->map[u.y + 1][u.x + 1]);
            }
        }

        if (u.y - 1 >= 0 && screen->map[u.y - 1][u.x].inHeap == true && screen->map[u.y - 1][u.x].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            screen->map[u.y - 1][u.x].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &screen->map[u.y - 1][u.x]);
        }

        if (u.y + 1 < 21 && screen->map[u.y + 1][u.x].inHeap == true && screen->map[u.y + 1][u.x].dist > screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType)) {
            screen->map[u.y + 1][u.x].dist = screen->map[u.y][u.x].dist + determineCost(screen->map[u.y][u.x].type, enemyType);

            heapifyUpCell(&mh, &screen->map[u.y + 1][u.x]);
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
    // For hikers
    if (npcType == 'H') {
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
    }

    // For rivals
    if (npcType == 'R') {
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
    }

    return INT_MAX;
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
            if (*heatMap->heatMap[i][j] == INT_MAX) {
                printf("   ");
            } else {
                printf("%02d ", *heatMap->heatMap[i][j] % 100);
            }
        }
        printf("\n");
    }
}


