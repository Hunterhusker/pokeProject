//
// Created by hunter on 4/16/22.
//

#include "world.h"
#include "mapBuilder.h"
#include <curses.h>
#include <cmath>

void gameBoardInit(world *world)
{
    world->currX = 199;
    world->currY = 199;

    for (int i = 0; i < 399; i++)
    {
        for (int j = 0; j < 399; j++)
        {
            if (world->board[i][j] != NULL)
            {
                world->board[i][j] = NULL;
            }
        }
    }
}

void printCurr(world *world)
{
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (world->board[world->currY][world->currX]->eMap[i][j] != NULL) {
                mvaddch(i + 1, j, world->board[world->currY][world->currX]->eMap[i][j]->type);
            } else {
                mvaddch(i + 1, j, world->board[world->currY][world->currX]->map[i][j].type);
            }
        }
    }

    //mvprintw(0, 0, "Current Location: (%d, %d): %s\n", world->currX, world->currY, str);
    //mvprintw(22, 0, "Use 10-key for movement; q to quit; check readme more controls");

    refresh(); // actually displays the board
}

void worldInit(world *world)
{
    gameBoardInit(world);

    world->board[world->currY][world->currX] = (map *) malloc(sizeof (map));

    generate(-1, -1, -1, -1, world->board[world->currY][world->currX], 100, world->entityCount);
}

void destroyWorld(world *world)
{
    for (int i = 0; i < 399; i++)
    {
        for (int j = 0; j < 399; j++)
        {
            if (world->board[i][j] != NULL)
            {
                deleteAllEntities(world->board[i][j]->eMap); // added deletion of all entities

                free(world->board[i][j]);
            }
        }
    }

    //endwin(); // de-init the screen so we can end the program
}

int isScreenGenerated(world *world, int x, int y)
{
    if (world->board[y][x] != NULL) {
        return 1;
    }

    return 0;
}

int buildingChance(int x, int y)
{
    int dist = abs(x - 199) + abs(y - 199);

    int chance = ((dist * -45) / 200) + 50;

    if (chance <= 0) {
        return 0;
    }

    return chance;
}

int goToLoc(world *world, int x, int y)
{
    int exits[4] = {-1, -1, -1, -1};

    if (x >= 399 || x < 0 || y >= 399 || y < 0)
    {
        return -1;
    }

    if (isScreenGenerated(world, x, y) == 1) {
        world->currX = x;
        world->currY = y;

        return 0;
    }

    if (x + 1 < 399 && isScreenGenerated(world, x + 1, y) == 1) {
        exits[3] = world->board[y][x+1]->exits[2][0];
    } else if (x + 1 >= 399) {
        // set no exit cond
        exits[3] = 0;
    }

    if (x - 1 >= 0 && isScreenGenerated(world, x - 1, y) == 1) {
        exits[2] = world->board[y][x-1]->exits[3][0];
    } else if (x - 1 < 0) {
        // set no exit cond
        exits[2] = 0;
    }

    if (y + 1 < 399 && isScreenGenerated(world, x, y + 1) == 1) {
        exits[1] = world->board[y+1][x]->exits[0][1];
    } else if (y + 1 >= 399) {
        // set no exit cond
        exits[1] = 0;
    }

    if (y - 1 >= 0 && isScreenGenerated(world, x, y - 1) == 1) {
        exits[0] = world->board[y-1][x]->exits[1][1];
    } else if (y - 1 < 0) {
        // set no exit cond
        exits[0] = 0;
    }

    world->board[y][x] = (map *) malloc(sizeof (map));

    generate(exits[0], exits[1], exits[2], exits[3], world->board[y][x], buildingChance(x, y), world->entityCount);

    world->currX = x;
    world->currY = y;

    return 0;
}