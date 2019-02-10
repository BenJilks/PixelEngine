
all:
	gcc src/*.c -Iinclude -lSDL2 -lvulkan -lm -o RayCast

