#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameData.h"
#include "CUnit-2.1-0/include/CUnit/console.h"

int InitSuit() {
    return 0;
}

int EndSuit() {
    return 0;
}

int Test_Store_Game(Game* game, FILE* fp, int expect) {
    int result;
    
    result = storeGame(game, fp);
    return result == expect;
}

void Test() {
    Game* game = NULL;
    FILE* fp = NULL;
    
    // storeGame
    CU_ASSERT(Test_Store_Game(game, fp, -1));
    game = (Game*) malloc(sizeof(Game));
    CU_ASSERT(Test_Store_Game(game, fp, -1));
    fp = fopen("test.dat", "w+");
    CU_ASSERT(Test_Store_Game(game, fp, -1));
    fclose(fp);
    free(game);
    game = createGame(10, 10, 0, 500);
    fp = fopen("test.dat", "w+");
    CU_ASSERT(Test_Store_Game(game, fp, 0));
}

int AddTestMainModule() {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("testSuite", InitSuite, EndSuite);
      
    if(NULL == pSuite)
        return 1;

    if( NULL == CU_add_test(pSuite, "Test", Test))
        return 1;

    return 0;
}
