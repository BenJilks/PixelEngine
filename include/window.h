#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL_keycode.h>
typedef void (*KeyEvent)(int key);
typedef void (*MouseEvent)(float dx, float dy);

int window_open(const char *title, int width, int height);
int window_should_close();
float window_delta_time();
void window_get_size(int *width, int *height);

void window_grab_mouse();
void window_force_close();

void window_on_key_down(KeyEvent event);
void window_on_key_up(KeyEvent event);
void window_on_mouse_move(MouseEvent event);
void window_bind_key(int id, int key_code);
int window_is_bind_pressed(int id);

void window_update();
void window_close();

#endif // WINDOW_H
