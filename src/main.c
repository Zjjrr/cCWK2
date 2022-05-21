#include <stdio.h>
#include <stdlib.h>
#include "GameData.h"
#include "Game.h"

int main(int argc, char** argv) {
    GameConfig* config = NULL;

    // for test
    config = load("stored01.dat");
    init();
    render();

    // game loop
    while(1) {
        input();
        update();
        render();
        SDL_Delay(config -> game -> delay);
    }
    
    return 0;
}
