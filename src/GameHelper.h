#ifndef _GAMEHELPER_H
#define _GAMEHELPER_H

#include <SDL2/SDL.h>

typedef enum _CellType {
    ALIVE,
    DEAD
} CellType;

SDL_Window *window = NULL;
SDL_Renderer* renderer = NULL;

void draw_game();
void draw_girdline();
void draw_cell(CellType type, int indexL, int indexR);

#endif
