#include <stdio.h>
#include <stdlib.h>
#include "GameData.h"
#include "Game.h"

int main(int argc, char** argv) {
    load("stored01.dat");
    init();
    getchar();
    return 0;
}
