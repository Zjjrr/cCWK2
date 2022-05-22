#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "GameData.h"

static unsigned int _getSize_(Game* game);

static void _nextStep_(Game* game);

static unsigned int** createArray(int l, int r);

static void freeArray(unsigned int** array, int l);

static void copyArray(unsigned int** source, unsigned int** target, int l, int r);

int storeGame(Game* game, FILE* file) {
    cJSON* json_root = NULL;
    cJSON* json_array_initial = NULL;
    cJSON* json_array_end = NULL;
    cJSON* json_array_temp = NULL;
    char* res = NULL;
    
    if (!file || !game)
        // file pointer or game pointer is invalid
        return -1;
    
    // create json_root as the head node of json
    json_root = cJSON_CreateObject();
    // append game datas
    cJSON_AddNumberToObject(json_root, KEY_WIDTH, game -> width);
    cJSON_AddNumberToObject(json_root, KEY_HEIGHT, game -> height);
    cJSON_AddNumberToObject(json_root, KEY_CURRENT_STEP, game -> currentStep);
    cJSON_AddNumberToObject(json_root, KEY_TOTAL_STEPS, game -> totalStep);
    cJSON_AddNumberToObject(json_root, KEY_DELAY, game -> delay);

    json_array_initial = cJSON_CreateArray();
    for (int i = 0; i < game -> height; i++) {
        json_array_temp = cJSON_CreateIntArray((int*) (*(game -> initialStatus + i)), game -> width);
        cJSON_AddItemToArray(json_array_initial, json_array_temp);
    }
    cJSON_AddItemToObject(json_root, KEY_INITIAL_STATUS, json_array_initial);
    
    json_array_end = cJSON_CreateArray();
    for (int i = 0; i < game -> height; i++) {
        json_array_temp = cJSON_CreateIntArray((int*) (*(game -> endStatus + i)), game -> width);
        cJSON_AddItemToArray(json_array_end, json_array_temp);
    }
    cJSON_AddItemToObject(json_root, KEY_END_STATUS, json_array_end);
    
    // try to generate json file
    if (!(res = cJSON_Print(json_root))) {
        // failed to print json
        cJSON_Delete(json_root);
        return -1;
    }

    // write json to file and release allocated resources
    fprintf(file, "%s", res);
    free(res);
    cJSON_Delete(json_root);
    
    return 0;
}

Game* restoreGame(FILE* file) {
    Game* game = NULL;
    int file_size = 0;
    unsigned int** status = NULL;
    char* buf = NULL;
    cJSON* json = NULL;
    cJSON* array = NULL;
    
    if (!file)
        // file is invalid
        return NULL;
    
    // Move the file pointer to the end of the file 
    // to get the number of characters in the file by offset
    // The buffer is then created dynamically
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    buf = (char*) malloc(file_size * sizeof(char));
    memset(buf, '\0', file_size * sizeof(char));
    
    // Go back to the file header and read in the file
    fseek(file, 0, SEEK_SET);
    fread(buf, sizeof(char), file_size, file);
    
    // Parsing JSON data
    if (!(json = cJSON_Parse(buf))) {
        // failed to parse json
        free(buf);
        return NULL;
    }
    
    // Create Game and restore datas
    game = (Game*) malloc(sizeof(Game));
    game -> width = (unsigned int) cJSON_GetNumberValue(cJSON_GetObjectItem(json, KEY_WIDTH));
    game -> height = (unsigned int) cJSON_GetNumberValue(cJSON_GetObjectItem(json, KEY_HEIGHT));
    game -> currentStep = (unsigned int) cJSON_GetNumberValue(cJSON_GetObjectItem(json, KEY_CURRENT_STEP));
    game -> totalStep = (unsigned int) cJSON_GetNumberValue(cJSON_GetObjectItem(json, KEY_TOTAL_STEPS));
    game -> delay = (unsigned int) cJSON_GetNumberValue(cJSON_GetObjectItem(json, KEY_DELAY));
    
    array = cJSON_GetObjectItem(json, KEY_INITIAL_STATUS);
    status = createArray(game -> height, game -> width);
    for (int i = 0; i < game -> height; i++) {
        for (int j = 0; j < game -> width; j++) {
            *(*(status + i) + j) = cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetArrayItem(array, i), j));
        }
    }
    game -> initialStatus = status;
    
    array = cJSON_GetObjectItem(json, KEY_END_STATUS);
    status = createArray(game -> height, game -> width);
    for (int i = 0; i < game -> height; i++)
        *(status + i) = (unsigned int*) malloc(game -> width * sizeof(unsigned int));
    for (int i = 0; i < game -> height; i++) {
        for (int j = 0; j < game -> width; j++) {
            *(*(status + i) + j) = cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetArrayItem(array, i), j));
        }
    }
    game -> endStatus = status;
    
    game -> getSize = _getSize_;
    game -> nextStep = _nextStep_;
    game -> release = detachGame;
    
    // release resources allocated
    free(buf);
    cJSON_Delete(json);
    
    return game;
}

Game* createGame(unsigned int width, unsigned int height, unsigned int totalSteps, unsigned int delay) {
    Game* game = NULL;
    
    if (width == 0 || width > MAX_WIDTH || height == 0 || height > MAX_HEIGHT)
        // parameters are invalid
        return NULL;
        
    // Create game dynamically based on parameters
    game = (Game*) malloc(sizeof(Game));
    game -> width = width;
    game -> height = height;
    game -> currentStep = 1;
    game -> totalStep = totalSteps;
    game -> delay = delay; // 1s
    game -> initialStatus = createArray(game -> height, game -> width);
    game -> endStatus = createArray(game -> height, game -> width);
    game -> getSize = _getSize_;
    game -> nextStep = _nextStep_;
    game -> release = detachGame;
    
    return game;
}

void detachGame(Game* game) {
    if (!game)
        exit(-1);
    
    freeArray(game -> initialStatus, game -> height);
    freeArray(game ->endStatus, game -> height);
    free(game);
}

static unsigned int _getSize_(Game* game) {
    return (unsigned int) (game -> width * game -> height);
}

static void _nextStep_(Game* game) {
    static int delta[] = {-1, 0, 1};
    unsigned int** status = NULL;
    unsigned int** temp = NULL;
    int alive = 0, x = 0, y = 0;
    
    status = game -> endStatus;
    if (game -> currentStep == 1)
        copyArray(game -> initialStatus, status, game -> height, game -> width);
    else if (game -> currentStep == 0) {
        copyArray(game -> initialStatus, status, game -> height, game -> width);
        game -> currentStep ++;
        return;
    }
    temp = createArray(game -> height, game -> width);
    copyArray(status, temp, game -> height, game -> width);
    
    for (int i = 0; i < game -> height; i++) {
        for (int j = 0; j < game -> width; j++) {
            alive = 0;
            for (int k = 0; k < 3; k++) {
                y = i + delta[k];
                for (int l = 0; l < 3; l++) {
                    x = j + delta[l];
                    // Check if the neighbor is out of range
                    if (x >= 0 && x < game -> width 
                        && y >= 0 && y < game -> height)
                        alive += *(*(status + y) + x);
                }
            }
            alive -= *(*(status + i) + j);
            // Change the state of the current cell
            if (alive == 3)
                *(*(temp + i) + j) = (unsigned int) SYMBOL_LIVE;
            else if (alive != 2)
                *(*(temp + i) + j) = (unsigned int) SYMBOL_DEAD;
        }
    }
    freeArray(status, game -> height);
    game -> endStatus = temp;
    game -> currentStep ++;
}

static unsigned int** createArray(int l, int r) {
    unsigned int** array = NULL;
    
    array = (unsigned int**) malloc(l * sizeof(unsigned int*));
    for (int i = 0; i < l; i++) {
        *(array + i) = (unsigned int*) malloc(r * sizeof(unsigned int));
        memset(*(array + i) , 0, r * sizeof(unsigned int));
    }
        
    return array;
}

static void freeArray(unsigned int** array, int l) {
    for (int i = 0; i < l; i++)
        free(*(array + i));
    free(array);
}

static void copyArray(unsigned int** source, unsigned int** target, int l, int r) {
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < r; j++)
            *(*(target + i) + j) = *(*(source + i) + j);
    }
}
