#ifndef _GAMEDATA_H
#define _GAMEDATA_H

#define KEY_WIDTH "width"
#define KEY_HEIGHT "height"
#define KEY_CURRENT_STEP "current_step"
#define KEY_TOTAL_STEPS "total_steps"
#define KEY_INITIAL_STATUS "inital_status"
#define KEY_END_STATUS "end_status"

#define SYMBOL_LIVE 0
#define SYMBOL_DEAD 1

#define MAX_WIDTH 512
#define MAX_HEIGHT 512

#include <stdio.h>

typedef struct _Game {
    // width of game range
    unsigned int width;
    // height of game range
    unsigned int height;
    // number of times currently in progress
    unsigned int currentStep;
    // total number of times, 0 means unlimited
    unsigned int totalStep;
    // initial game state
    unsigned int** initialStatus;
    // current game status
    unsigned int** endStatus;
    
    // get the size of the game range
    unsigned int (*getSize) (void);
    // proceed to the next step
    void (*nextStep) (struct _Game*);
    // release any resource allocated
    void (*release) (struct _Game*);
} Game;

// store game status to file, return 0 if successful, -1 otherwise
int storeGame(Game* game, FILE* file);
// restore game status from file, return NULL if failed
Game* restoreGame(FILE* file);
// create a game with parameters, return NULL if failed
// values other than 0 are valid for width and height
Game* createGame(unsigned int width, unsigned int height, unsigned int totalSteps);
// release resources that allocated for the game
void detatchGame(Game* game);

#endif
