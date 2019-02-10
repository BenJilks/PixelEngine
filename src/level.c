#include "level.h"
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#define CHUNK_SIZE 21
#define MIN(a, b) (a) < (b) ? (a) : (b)

static Wall *level = NULL;
static int level_size = 0;
static int max_size = 0;

void level_new()
{
    if (level == NULL)
    {
        level = malloc(sizeof(Wall) * CHUNK_SIZE);
        max_size = CHUNK_SIZE;
    }

    level_size = 0;
}

static void check_level_size(int len)
{
    if (level_size + len >= max_size)
    {
        max_size += len + CHUNK_SIZE;
        level = realloc(level, sizeof(Wall) * max_size);
    }
}

float distance(float x1, float y1, float x2, float y2)
{
    float a, b;
    a = x2 - x1;
    b = y2 - y1;
    return sqrtf(a*a + b*b);
}

void level_add_wall(float x1, float y1, float x2, float y2)
{
    check_level_size(1);
    
    Wall wall;
    wall.x1 = x1; 
    wall.y1 = y1;
    wall.x2 = x2; 
    wall.y2 = y2;
    level[level_size++] = wall;
}

void level_close()
{
    free(level);
}
