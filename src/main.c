#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "level.h"
#include "render.h"

int main()
{
	level_new();
	level_add_wall(-4, 4, 4, 4);
	level_add_wall(-4, -4, 4, -4);
	level_add_wall(-4, 4, -4, -4);
	level_add_wall(4, 4, 4, 0);
	level_add_wall(4, -1, 4, -4);
	level_add_wall(4, 0, 5, 0);
	level_add_wall(4, -1, 5, -1);
	level_add_wall(5, 4, 5, 0);
	level_add_wall(5, -1, 5, -4);
	level_add_wall(5, 4, 9, 4);
	level_add_wall(5, -4, 6, -4);
	level_add_wall(9, 4, 6, -4);

	if (!window_open("RayCast", 1920*0.5, 1080*0.5))
	{
		level_close();
		return -1;
	}
	
	int i, j;
	while (!window_should_close())
	{
		window_update();
	}
	
	level_close();
	window_close();
	return 0;
}
