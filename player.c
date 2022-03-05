#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mapBuilder.h"

/**
 * Places a player randomly on a road in the screen
 *
 * @param map ~ the screen to place the player within
 * @param player ~ the player to place in the game
 */
void placePlayer(map_t *map, cell_t *player)
{
    srand(time(NULL)); // Set the seed again

    int loc, dir, found = 0;

    while (found == 0) {

        dir = rand() % 2;

        if (dir == 0) {
            loc = (rand() % 78) + 2;

            for (int i = 1; i < 21; i++) {
                if (map->map[i][loc].type == '#') {
                    player->y = i;
                    player->x = loc;

                    //player->loc = map->map[i][loc].type;
                    //map->map[i][loc].type = '@';

                    map->eMap[i][loc] = player;

                    found = 1;
                    break;
                }
            }
        } else {
            loc = (rand() % 19) + 2;

            for (int i = 1; i < 80; i++) {
                if (map->map[loc][i].type == '#') {
                    player->y = loc;
                    player->x = i;

                    //player->loc = map->map[loc][i].type;
                    //map->map[loc][i].type = '@';

                    map->eMap[loc][i] = player;

                    found = 1;
                    break;
                }
            }
        }
    }
}

/**
 * Unplaces the player so it is in only one spot, and so we don't lose data in our screen
 *
 * @param map ~ the screen to place the player into
 * @param player ~ the player to place
 */
void unplacePlayer(map_t *map, cell_t *player)
{
    // Return the old char to it's place
    //map->map[player->y][player->x].type = player->loc;

    map->eMap[player->y][player->x] = NULL;
}