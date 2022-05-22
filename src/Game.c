#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "Game.h"
#include "GameHelper.h"
#include "GameData.h"
#include "GameGenerator.h"

static GameConfig* config = NULL;
extern int errno;

// load status file
GameConfig* load(char* file) {
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
    rect -> w = 30;
    rect -> h = 30;
    // init gameRect
    rect = &(config -> gameRect);
    rect -> x= 0;
    rect -> y = 0;
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
    
    fclose(fp);
    return config;
}

// initialise the game data
void init() {
    if (!config) {
        fprintf(stderr, "Game_Init: failed to get config file.\n");
        exit(-1);
    }
    
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        quit();
    }
    
    // Register SDL_Quit callback when exit
    atexit(SDL_Quit);
    
    // Create SDL Window
    window = SDL_CreateWindow(
        APP_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        (config -> windowRect).w,
        (config -> windowRect).h,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        quit();
    }
    
    // Create and initialize Renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

// read keyboard input
void input() {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    
    SDL_PumpEvents();
    
    // Update key status
    esc_pressed = state[SDL_SCANCODE_ESCAPE];
    enter_pressed = state[SDL_SCANCODE_RETURN];
    space_pressed = state[SDL_SCANCODE_SPACE];
    mouse_primer_pressed = 0;
    // handle quit event
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            esc_pressed = 1;
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_RELEASED && event.button.clicks == 1) {
                mouse_primer_pressed = 1;
                pointer_x = event.button.x;
                pointer_y = event.button.y;
            }
        }
    }
}

// update game status
void update() {
    static int end = 0;
    Game* game = config -> game;
    
    // esc pressed
    if (esc_pressed)
        quit();
    
    // space pressed
    if (space_pressed) {
        game -> currentStep = 0;
        end = 0;
        enter_pressed = 1;
        fprintf(stdout, "Game: Replay\n");
    }
    
    if (!game -> totalStep) {
        // manual
        if (enter_pressed) {
            game -> nextStep(game);
         fprintf(stdout, "Game: ManualPlay [Current]-[%u]\n", game -> currentStep);
        }
    } else if (game -> currentStep < game -> totalStep) {
        // auto
        game -> nextStep(game);
        fprintf(stdout, "Game: AutoPlay [Current/Total]-[%u/%u]\n", game -> currentStep, game -> totalStep);
    } else if (!end) {
        fprintf(stdout, "Game: Reach to the end.\n");
        end = 1;
    }
}

// update
void update_generator() {
    Game* game = config -> game;
    int l, r, s;
    
    if (enter_pressed || esc_pressed)
        quit();
    
    if (mouse_primer_pressed) {
        l = (int)(pointer_y / (config -> cellRect).h);
        r = (int)(pointer_x / (config -> cellRect).w);
        s = ! *(*(game -> initialStatus + l) + r);
        *(*(game -> initialStatus + l) + r) = s;
        game -> currentStep = 0;
        game -> nextStep(game);
        fprintf(stdout, "Game: Set %d to (%d, %d)\n", s, r, l);
    }
}

// render
void render_generator() {
    draw_game(&(config -> game -> initialStatus));
    SDL_RenderPresent(renderer);
}

void render() {
    draw_game(&(config -> game -> endStatus));
    SDL_RenderPresent(renderer);
}

// quit the game
void quit() {
    Game* game = config -> game;
    FILE* fp = fopen(config -> file, "w+");
    
    // store status to file
    storeGame(config -> game, fp);
    fclose(fp);
    fprintf(stdout, "Game: Status stored to %s\nGame: Quit\n", config -> file);
    
    // release memory
    game -> release(game);
    free(config);
    
    exit(0);
}

void draw_game(unsigned int*** status_p) {
    Game* game = config -> game;
    unsigned int** status = *status_p;

    // Draw game area
    for (int i = 0; i < game -> height; i++) {
        for (int j = 0; j < game -> width; j++) {
            draw_cell(
                *(*(status + i) + j) ? ALIVE : DEAD, // Cell type
                i,                                      // line(l)
                j                                       // row(r)
            );
        }
    }
    
    draw_girdline();
}

void draw_girdline() {
    Game* game = config -> game;
    SDL_Rect* cellRect = &(config -> cellRect);
    SDL_Rect* gameRect = &(config -> gameRect);
    
    SDL_SetRenderDrawColor(renderer, 123, 123, 123, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < game -> width; i++)
        SDL_RenderFillRect(renderer, &(SDL_Rect) {
                cellRect -> w * i + gameRect -> x,  // x location
                gameRect -> y,                      // y location
                2,                                  // width
                gameRect -> h                       // height
        });
    for (int i = 0; i < game -> height; i++)
        SDL_RenderFillRect(renderer, &(SDL_Rect) {
                gameRect -> x,
                cellRect -> h * i + gameRect -> y,
                gameRect -> w,
                2
        });
}

void draw_cell(CellType type, int indexL, int indexR) {
    SDL_Rect* cellRect = &(config -> cellRect);
    SDL_Rect* gameRect = &(config -> gameRect);

    switch (type) {
        case ALIVE:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            break;
        case DEAD:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            break;
    }
    SDL_RenderFillRect(renderer, &(SDL_Rect){
            cellRect -> w * indexR + gameRect -> x,
            cellRect -> h * indexL + gameRect -> y,
            cellRect -> w,
            cellRect -> h
    });
}
