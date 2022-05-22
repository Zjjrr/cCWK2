#ifndef _GAMEHELPER_H
#define _GAMEHELPER_H

#include <SDL2/SDL.h>

int esc_pressed;
int enter_pressed;
int space_pressed;
int mouse_primer_pressed;
int pointer_x;
int pointer_y;

typedef enum _CellType {
    ALIVE,
    DEAD
} CellType;

SDL_Window *window = NULL;
SDL_Renderer* renderer = NULL;


void draw_game(unsigned int*** status_p);
void draw_girdline();
void draw_cell(CellType type, int indexL, int indexR);

#endif
