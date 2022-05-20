#ifndef _GAME_H
#define _GAME_H

#include <SDL2/SDL.h>

#include "GameData.h"

#define APP_TITLE "Game of Life"

typedef struct _GameConfig {
    Game* game; // Game Data
    char* file; // Archive location
    
    SDL_Rect windowRect; // Abstraction of window display area
    SDL_Rect statusRect; // Abstraction of status display area
    SDL_Rect gameRect; // Abstraction of game area
    SDL_Rect cellRect; // Abstraction of cell
} GameConfig;

void load(char* file);
void init();
void input();
void update();
void render();
void quit();

#endif
