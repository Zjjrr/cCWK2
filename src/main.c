#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GameData.h"
#include "Game.h"
#include "GameGenerator.h"

static void startGame(char* path);

static void generateGame(char** argv);

int main(int argc, char** argv) {
    if (argc == 3 && !strcmp(argv[1], "load")) {
        fprintf(stdout, "Tips:\n\t[SPACE]\tReplay\n\t[ENTER]\tManual Play\n\t[ESC]\tSave and Quit\n\n");
        startGame(argv[2]);
    } else if (argc == 7 && !strcmp(argv[1], "make")) {
        fprintf(stdout, "Tips:\n\t[Click]\tSet Status\n\t[ENTER]\tSave and Quit\n\n");
        generateGame(argv);
    } else
        fprintf(stdout, "Invalid Parameters.\n\t"
                        "Example(1): load <file_path>\n\t"
                        "Example(2): create <file_path> <width> <height> <steps> <delay(ms)>\n");

    return 0;
}

static void startGame(char* path) {
    GameConfig* config = NULL;

    config = load(path);
    init();
    render();

    // game loop
    while(1) {
        input();
        update();
        render();
        if (config -> game -> totalStep)
            SDL_Delay(config -> game -> delay);
        else
            SDL_Delay(80);
    }
}

static void generateGame(char** argv) {
    Game* game = NULL;
    FILE* fp = NULL;
    
    game = createGame(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    fp = fopen(argv[2], "w+");
    if (!storeGame(game, fp))
        fprintf(stdout, "Game: Create status file successfully\n");
    else
        fprintf(stderr, "Game: failed to create status file\n");
    fclose(fp);
    
    load(argv[2]);
    init();
    render();
    
    // game loop
    while(1) {
        input();
        update_generator();
        render_generator();
        SDL_Delay(80);
    }
}
