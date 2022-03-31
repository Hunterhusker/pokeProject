#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include "minHeap.h"
#include "mapBuilder.h"

void printMap(map *screen)
{
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            if (screen->eMap[i][j] == NULL) {
                printf("%c", screen->map[i][j].type);
            } else {
                printf("%c", screen->eMap[i][j]->type);
            }

        }

        printf("\n");
    }
}

void emptyEntityMap(cell *eMap[21][80])
{
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            eMap[i][j] = NULL;
        }
    }
}

/**
 * Free all of the declared entities
 * @param eMap ~ the entity map
 */
void deleteAllEntities(cell *eMap[21][80])
{
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (eMap[i][j] != NULL) {
                free(eMap[i][j]);
            }
        }
    }
}

void emptyGrid(cell map[21][80])
{
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            if (i == 0 || i == 20 || j == 0 || j == 79) {
                map[i][j].type = '%';
                map[i][j].weight = 100; // arbitrarily large value for an impassible weight
                map[i][j].dist = INT_MAX;
                map[i][j].y = i;
                map[i][j].x = j;
            } else {
                map[i][j].type = 'x';
                map[i][j].dist = INT_MAX;
                map[i][j].y = i;
                map[i][j].x = j;
            }
        }
    }
}

void placeExits(cell map[21][80], int exits[4][2], int e1, int e2, int e3, int e4)
{
    srand(time(NULL));
    
    if (e1 == -1)
    {
        e1 = rand() % 72 + 4;
    }

    if (e1 != 0) {
        map[0][e1].type = '#';
        map[0][e1].weight = '0';
    }

    exits[0][0] = 0;
    exits[0][1] = e1;

    if (e3 == -1)
    {
        e3 = rand() % 13 + 4;
    }

    if (e3 != 0) {
        map[e3][0].type = '#';
        map[e3][0].weight = '0';
    }

    exits[2][0] = e3;
    exits[2][1] = 0;

    if (e2 == -1)
    {
        e2 = rand() % 72 + 4;
    }

    if (e2 != 0) {
        map[20][e2].type = '#';
        map[20][e2].weight = '0';
    }

    exits[1][0] = 20;
    exits[1][1] = e2;

    if (e4 == -1)
    {
        e4 = rand() % 13 + 4;
    }

    if (e4 != 0) {
        map[e4][79].type = '#';
        map[e4][79].weight = '0';
    }

    exits[3][0] = e4;
    exits[3][1] = 79;
}

int placeBiomes(cell map[21][80], int filledTiles[][2])
{
    int grassAmt, clearingAmt, tmp_x, tmp_y, biomeCnt;
    biomeCnt = 0;

    srand(time(NULL));

    // How many of each biome do we want
    grassAmt = rand() % 4 + 4;
    clearingAmt = rand() % 4 + 10;

    for (int i = 0; i < grassAmt; i++) {
        // Gen the x and y for the starting pos
        tmp_x = rand() % 78 + 1;
        tmp_y = rand() % 19 + 1;

        // Put it in the map
        map[tmp_y][tmp_x].type = ':';
        map[tmp_y][tmp_x].weight = 5;
        map[tmp_y][tmp_x].instance = i;

        // Add the coords to the list of all generated coords
        filledTiles[biomeCnt][0] = tmp_y;
        filledTiles[biomeCnt][1] = tmp_x;

        biomeCnt++;
    }

    for (int i = 0; i < clearingAmt; i++) {
        // Gen the x and y for the starting pos
        tmp_x = rand() % 78 + 1;
        tmp_y = rand() % 19 + 1;

        // Put it in the map
        map[tmp_y][tmp_x].type = '.';
        map[tmp_y][tmp_x].weight = 5;
        map[tmp_y][tmp_x].instance = i;

        // Add the coords to the list of all generated coords
        filledTiles[biomeCnt][0] = tmp_y;
        filledTiles[biomeCnt][1] = tmp_x;

        biomeCnt++;
    }

    return biomeCnt;
}

void copyCell(cell *a, cell *b)
{
    b->type = a->type;
    b->weight = a->weight;
    b->instance = a->instance;
}

void grow(cell map[21][80], int filledTiles[1482][2], int curr)
{
    // Control the loop by checking to see if there is more to grow into
    int cursor = 0;
    int x, y;

    while (cursor < 1482)  // While we still need to grow
    {

        y = filledTiles[cursor][0];
        x = filledTiles[cursor][1];

        if (x != 0 && y != 0 && map[y][x].type != '%' && map[y][x].type != 'x')
        {
            if (y - 1 > 0 && map[y-1][x].type == 'x') {
                copyCell(&map[y][x], &map[y - 1][x]);

                filledTiles[curr][0] = y - 1;
                filledTiles[curr][1] = x;

                curr++;
            }
            else if (y - 1 > 0 && map[y-1][x].type != 'x' && map[y-1][x].type != '%')
            {
                if ((map[y-1][x].type == map[y][x].type && map[y-1][x].instance != map[y][x].instance) || map[y-1][x].type != map[y][x].type)
                {
                    map[y][x].weight = 1;
                }
            }

            if (y + 1 < 20 && map[y+1][x].type == 'x') {
                copyCell(&map[y][x], &map[y + 1][x]);

                filledTiles[curr][0] = y + 1;
                filledTiles[curr][1] = x;

                curr++;
            }
            else if (y + 1 > 0 && map[y+1][x].type != 'x' && map[y+1][x].type != '%')
            {
                if ((map[y+1][x].type == map[y][x].type && map[y+1][x].instance != map[y][x].instance) || map[y+1][x].type != map[y][x].type)
                {
                    map[y][x].weight = 1;
                }
            }

            if (x - 1 > 0 && map[y][x-1].type == 'x') {
                copyCell(&map[y][x], &map[y][x - 1]);

                filledTiles[curr][0] = y;
                filledTiles[curr][1] = x - 1;

                curr++;
            }
            else if (x - 1 > 0 && map[y][x-1].type != 'x' && map[y][x-1].type != '%')
            {
                if ((map[y][x-1].type == map[y][x].type && map[y][x-1].instance != map[y][x].instance) || map[y][x-1].type != map[y][x].type)
                {
                    map[y][x].weight = 1;
                }
            }

            if (x + 1 < 79 && map[y][x+1].type == 'x') {
                copyCell(&map[y][x], &map[y][x + 1]);

                filledTiles[curr][0] = y;
                filledTiles[curr][1] = x + 1;

                curr++;
            }
            else if (x + 1 > 0 && map[y][x+1].type != 'x' && map[y][x+1].type != '%')
            {
                if ((map[y][x+1].type == map[y][x].type && map[y][x+1].instance != map[y][x].instance) || map[y][x+1].type != map[y][x].type)
                {
                    map[y][x].weight = 1;
                }
            }

            if (x - 2 > 0 && map[y][x-2].type == 'x' && map[y][x-1].type == map[y][x].type) {
                copyCell(&map[y][x], &map[y][x - 2]);

                filledTiles[curr][0] = y;
                filledTiles[curr][1] = x - 1;

                curr++;
            }

            if (x + 2 < 79 && map[y][x+2].type == 'x' && map[y][x+1].type == map[y][x].type) {
                copyCell(&map[y][x], &map[y][x + 2]);

                filledTiles[curr][0] = y;
                filledTiles[curr][1] = x + 2;

                curr++;
            }

            cursor++;
        }
    }
}

void findNeighbors(cell map[21][80], cell *cell, int neighbors[4][2])
{
    int x, y, curr = 0;

    x = cell->x;
    y = cell->y;

    if (x - 1 >= 0 && y < 20 && y >= 0 && map[y][x-1].type != '%')
    {
        neighbors[curr][0] = y;
        neighbors[curr][1] = x - 1;

        curr++;
    }

    if (x + 1 < 80 && y < 20 && y >= 0 && map[y][x+1].type != '%')
    {
        neighbors[curr][0] = y;
        neighbors[curr][1] = x + 1;

        curr++;
    }

    if (y - 1 >= 0 && x < 79 && x >= 0 && map[y-1][x].type != '%')
    {
        neighbors[curr][0] = y - 1;
        neighbors[curr][1] = x;

        curr++;
    }

    if (y + 1 < 21 && x < 79 && x >= 0 && map[y+1][x].type != '%')
    {
        neighbors[curr][0] = y + 1;
        neighbors[curr][1] = x;

        curr++;
    }

    for (int i = curr; i < 4; i++)
    {
        neighbors[i][0] = -1;
        neighbors[i][1] = -1;
    }
}

void dijkstra(int x1, int y1, int x2, int y2, cell map[21][80])
{
    mhResetMap(map);

    minHeap mh;
    mh.currLen = 0;

    int found = 0;
    int neighbors[4][2];
    int nx, ny;

    // Set the source and dest info to 0
    map[y1][x1].dist = 0;

    map[y2][x2].weight = 1;
    map[y1][x1].weight = 1;

    map[y1][x1].parentLoc[0] = -1;
    map[y1][x1].parentLoc[1] = -1;

    findNeighbors(map, &map[y2][x2], neighbors);

    mhAddAll(&mh, map);

    while (found == 0)
    {
        cell u = mhExtract(&mh);
        u.inHeap = false;

        if (u.y == y2 && u.x == x2) {
            found = 1;
            break;
        }

        findNeighbors(map, &u, neighbors);

        for (int i = 0; i < 4; i++) {
            if (neighbors[i][0] >= 0 && neighbors[i][1] >= 0 && neighbors[i][0] < 21 && neighbors[i][1] < 80)
            {
                nx = neighbors[i][1];
                ny = neighbors[i][0];

                if (map[ny][nx].inHeap == true)
                {
                    if (map[ny][nx].dist > (u.dist + map[ny][nx].weight))
                    {
                        map[ny][nx].dist = u.dist + map[ny][nx].weight;

                        map[ny][nx].parentLoc[0] = u.y;
                        map[ny][nx].parentLoc[1] = u.x;

                        heapifyUpCell(&mh, &map[ny][nx]);
                    }
                }
            }
        }
    }

    cell *n = &map[y2][x2];
    int px, py;
    
    px = n->parentLoc[1];
    py = n->parentLoc[0];

    while (px != -1 && py != -1) {
        n = &map[py][px];

        n->type = '#';

        px = n->parentLoc[1];
        py = n->parentLoc[0];
    }
}

void layPath(cell map[21][80], int exits[4][2])
{

    if (exits[0][1] == 0) {
        if (exits[2][0] != 0) {
            // do a little trolling and loop to a side exit
            exits[0][1] = exits[2][1];
            exits[0][0] = exits[2][0];
        } else {
            // if we were in a corner before then loop to the other side, and if both are 0, then life is hell bc something else is much more wrong
            exits[0][1] = exits[3][1];
            exits[0][0] = exits[3][0];
        }
    }

    if (exits[1][1] == 0) {
        if (exits[2][0] != 0) {
            exits[1][1] = exits[2][1];
            exits[1][0] = exits[2][0];
        } else {
            // if we were in a corner before then loop to the other side, and if both are 0, then life is hell bc something else is much more wrong
            exits[1][1] = exits[3][1];
            exits[1][0] = exits[3][0];
        }
    }

    if (exits[2][0] == 0) {
        if (exits[1][1] != 0) {
            // do a little trolling and loop to a side exit
            exits[2][1] = exits[1][1];
            exits[2][0] = exits[1][0];
        } else {
            // if we were in a corner before then loop to the other side, and if both are 0, then life is hell bc something else is much more wrong
            exits[2][1] = exits[0][1];
            exits[2][0] = exits[0][0];
        }
    }

    if (exits[3][0] == 0) {
        if (exits[1][1] != 0) {
            // do a little trolling and loop to a side exit
            exits[3][1] = exits[1][1];
            exits[3][0] = exits[1][0];
        } else {
            // if we were in a corner before then loop to the other side, and if both are 0, then life is hell bc something else is much more wrong
            exits[3][1] = exits[0][1];
            exits[3][0] = exits[0][0];
        }
    }

    dijkstra(exits[0][1], exits[0][0], exits[1][1], exits[1][0], map);

    dijkstra(exits[2][1], exits[2][0], exits[3][1], exits[3][0], map);
}

void addShop(cell map[21][80], char shopIcon)
{
    int dir, pos, loopCond = 1;

    dir = rand() % 2;

    if (dir == 1) {

        while (loopCond == 1) {
            pos = rand() % 70 + 4;

            for (int i = 3; i < 18; i++) {
                if (map[i+1][pos].type == '#') {
                    if (map[i][pos - 1].type != '#' && map[i][pos].type != '#' && map[i - 1][pos - 1].type != '#' && map[i - 1][pos].type != '#' && map[i][pos - 1].type != 'M' && map[i][pos].type != 'M' && map[i - 1][pos - 1].type != 'M' && map[i - 1][pos].type != 'M') {
                        map[i][pos].type = shopIcon;
                        map[i][pos-1].type = shopIcon;
                        map[i-1][pos].type = shopIcon;
                        map[i-1][pos-1].type = shopIcon;

                        loopCond = 0;
                        break;
                    } else if (map[i][pos + 1].type != '#' && map[i][pos].type != '#' && map[i - 1][pos + 1].type != '#' && map[i - 1][pos].type != '#' && map[i][pos + 1].type != 'M' && map[i][pos].type != 'M' && map[i - 1][pos + 1].type != 'M' && map[i - 1][pos].type != 'M') {
                        map[i][pos].type = shopIcon;
                        map[i][pos+1].type = shopIcon;
                        map[i-1][pos].type = shopIcon;
                        map[i-1][pos+1].type = shopIcon;

                        loopCond = 0;
                        break;
                    }
                }
            }

            if (loopCond == 1) {
                for (int i = 18; i > 3; i--) {
                    if (map[i-1][pos].type == '#') {
                        if (map[i][pos - 1].type != '#' && map[i][pos].type != '#' && map[i + 1][pos - 1].type != '#' && map[i + 1][pos].type != '#' && map[i][pos - 1].type != 'M' && map[i][pos].type != 'M' && map[i + 1][pos - 1].type != 'M' && map[i + 1][pos].type != 'M') {
                            map[i][pos].type = shopIcon;
                            map[i][pos-1].type = shopIcon;
                            map[i+1][pos].type = shopIcon;
                            map[i+1][pos-1].type = shopIcon;

                            loopCond = 0;
                            break;
                        } else if (map[i][pos + 1].type != '#' && map[i][pos].type != '#' && map[i + 1][pos + 1].type != '#' && map[i + 1][pos].type != '#' && map[i][pos + 1].type != 'M' && map[i][pos].type != 'M' && map[i + 1][pos + 1].type != 'M' && map[i + 1][pos].type != 'M') {
                            map[i][pos].type = shopIcon;
                            map[i][pos+1].type = shopIcon;
                            map[i+1][pos].type = shopIcon;
                            map[i+1][pos+1].type = shopIcon;

                            loopCond = 0;
                            break;
                        }
                    }
                }
            }

        }

    } else {
        while (loopCond == 1) {
            pos = rand() % 15 + 3;

            for (int i = 3; i < 77; i++) {
                if (map[pos][i+1].type == '#') {
                    if (map[pos - 1][i].type != '#' && map[pos][i].type != '#' && map[pos - 1][i - 1].type != '#' && map[pos][i - 1].type != '#' && map[pos - 1][i].type != 'M' && map[pos][i].type != 'M' && map[pos - 1][i - 1].type != 'M' && map[pos][i - 1].type != 'M') {
                        map[pos][i].type = shopIcon;
                        map[pos-1][i].type = shopIcon;
                        map[pos][i-1].type = shopIcon;
                        map[pos-1][i-1].type = shopIcon;

                        loopCond = 0;
                        break;
                    } else if (map[pos + 1][i].type != '#' && map[pos][i].type != '#' && map[pos + 1][i - 1].type != '#' && map[pos][i - 1].type != '#' && map[pos + 1][i].type != 'M' && map[pos][i].type != 'M' && map[pos + 1][i - 1].type != 'M' && map[pos][i - 1].type != 'M') {
                        map[pos][i].type = shopIcon;
                        map[pos+1][i].type = shopIcon;
                        map[pos][i-1].type = shopIcon;
                        map[pos+1][i-1].type = shopIcon;

                        loopCond = 0;
                        break;
                    }
                }
            }

            if (loopCond == 1) {
                for (int i = 77; i > 3; i--) {
                    if (map[pos][i-1].type == '#') {
                        if (map[pos - 1][i].type != '#' && map[pos][i].type != '#' && map[pos - 1][i + 1].type != '#' && map[pos][i + 1].type != '#' && map[pos - 1][i].type != 'M' && map[pos][i].type != 'M' && map[pos - 1][i + 1].type != 'M' && map[pos][i + 1].type != 'M') {
                            map[pos][i].type = shopIcon;
                            map[pos-1][i].type = shopIcon;
                            map[pos][i+1].type = shopIcon;
                            map[pos-1][i+1].type = shopIcon;

                            loopCond = 0;
                            break;
                        } else if (map[pos + 1][i].type != '#' && map[pos][i].type != '#' && map[pos + 1][i + 1].type != '#' && map[pos][i + 1].type != '#' && map[pos + 1][i].type != 'M' && map[pos][i].type != 'M' && map[pos + 1][i + 1].type != 'M' && map[pos][i + 1].type != 'M') {
                            map[pos][i].type = shopIcon;
                            map[pos+1][i].type = shopIcon;
                            map[pos][i+1].type = shopIcon;
                            map[pos+1][i+1].type = shopIcon;

                            loopCond = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
}

int placeEntities(int entityCount, map *screen, minHeap *mh)
{
    int type;

    for (int i = 0; i < entityCount; i++) {
        type = rand() % 6;

        switch (type) {
            case 0: // make a hiker
                placeEntity(screen, mh, 'h');
                break;

            case 1: // make a pacer
                placeEntity(screen, mh, 'p');
                break;

            case 2: // make a wanderer
                placeEntity(screen, mh, 'w');
                break;

            case 3: // make a stationary
                placeEntity(screen, mh, 's');
                break;

            case 4: // make a random walker
                placeEntity(screen, mh, 'n');
                break;

            case 5: // make a rival walker
                placeEntity(screen, mh, 'r');
                break;

            default: // shit pants
                return -1;
        }
    }

    return 0;
}

void generate(int e1, int e2, int e3, int e4, map *board, int shopChance, int entityCount) {
    srand(time(NULL));

    int filledTiles[1482][2];
    int currFilledAmt;
    int mChance = 1, cChance = 1; // just anything not 0 so if we don't want to do it we don't

    // Set the current length of the internal minHeap to be 0
    board->mh.currLen = 0;

    emptyGrid(board->map);
    emptyEntityMap(board->eMap);
    
    placeExits(board->map, board->exits, e1, e2, e3, e4);
    
    currFilledAmt = placeBiomes(board->map, filledTiles);
    
    grow(board->map, filledTiles, currFilledAmt);

    layPath(board->map, board->exits);

    if (shopChance != 0) {
        mChance = rand() % 100 / shopChance;
        cChance = rand() % 100 / shopChance;
    }

    if (mChance == 0) {
        addShop(board->map, 'M');
    }

    if (cChance == 0) {
        addShop(board->map, 'C');
    }

    placeEntities(entityCount, board, &board->mh);
}
