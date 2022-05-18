#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "Game.h"
#include "GameHelper.h"
#include "GameData.h"

static GameConfig* config = NULL;
extern int errno;

void load(char* file) {
    FILE* fp = NULL;
    Game* game = NULL;
    SDL_Rect* rect = NULL;
    
    if (!(fp = fopen(file, "r"))) {
        fprintf(stderr, "Game_Loader: failed to load status file. Errno: %d\n", errno);
        exit(-1);
    }
    
    if (!(game = restoreGame(fp))) {
        fprintf(stderr, "Game_Loader: failed to load game.\n");
        exit(-1);
    }
    
    config = (GameConfig*) malloc(sizeof(GameConfig));
    config -> game = game;
    config -> file = file;
    // init cellRect
    rect = &(config -> cellRect);
    rect -> x = 0;
    rect -> y = 0;
    rect -> w = 20;
    rect -> h = 20;
    // init gameRect
    rect = &(config -> gameRect);
    rect -> x= 0;
    rect -> y = 10;
    rect -> w = (config -> cellRect).w * config -> game -> width;
    rect -> h = (config -> cellRect).h * config -> game -> height;
    // init statusRect
    rect = &(config -> statusRect);
    rect -> x = 0;
    rect -> y = 0;
    rect -> w = (config -> gameRect).w;
    rect -> h = (config -> gameRect).y;
    // init windowRect
    rect = &(config -> windowRect);
    rect -> x = 0;
    rect -> y = 0;
    rect -> w = (config -> gameRect).w;
    rect -> h = (config -> gameRect).h + (config -> statusRect).h;
}

void init() {
    if (!config) {
        fprintf(stderr, "Game_Init: failed to get config file.\n");
        exit(-1);
    }
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    
    atexit(SDL_Quit);
    
    SDL_CreateWindowAndRenderer((config -> windowRect).w, (config -> windowRect).h, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Game of Life");
}
