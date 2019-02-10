#ifndef LEVEL_H
#define LEVEL_H

typedef struct Wall
{
    float x1, y1;
    float x2, y2;
} Wall;

void level_new();
void level_add_wall(float x1, float y1, float x2, float y2);

void level_close();

#endif // LEVEL_H
