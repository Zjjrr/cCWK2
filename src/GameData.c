#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "GameData.h"

static unsigned int _getSize_(Game* game);

static void _nextStep_(Game* game);

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
    
    array = cJSON_GetObjectItem(json, KEY_INITIAL_STATUS);
    status = (unsigned int**) malloc(game -> height * sizeof(unsigned int*));
    for (int i = 0; i < game -> height; i++)
        *(status + i) = (unsigned int*) malloc(game -> width * sizeof(unsigned int));
    for (int i = 0; i < game -> height; i++) {
        for (int j = 0; j < game -> width; j++) {
            *(*(status + i) + j) = cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetArrayItem(array, i), j));
        }
    }
    game -> initialStatus = status;
    
    array = cJSON_GetObjectItem(json, KEY_END_STATUS);
    status = (unsigned int**) malloc(game -> height * sizeof(unsigned int*));
    for (int i = 0; i < game -> height; i++)
        *(status + i) = (unsigned int*) malloc(game -> width * sizeof(unsigned int));
    for (int i = 0; i < game -> height; i++) {
        for (int j = 0; j < game -> width; j++) {
            *(*(status + i) + j) = cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetArrayItem(array, i), j));
        }
    }
    game -> endStatus = status;
    
    // release resources allocated
    free(buf);
    cJSON_Delete(json);
    
    return game;
}

Game* createGame(unsigned int width, unsigned int height, unsigned int totalSteps) {
    Game* game = NULL;
    unsigned int** tmp = NULL;
    
    if (width == 0 || width > MAX_WIDTH || height == 0 || height > MAX_HEIGHT)
        // parameters are invalid
        return NULL;
        
    // Create game dynamically based on parameters
    game = (Game*) malloc(sizeof(Game));
    game -> width = width;
    game -> height = height;
    game -> currentStep = 0;
    game -> totalStep = totalSteps;
    game -> initialStatus = (unsigned int**) malloc(height * sizeof(unsigned int*));
    for (int i = 0; i < height; i++) {
        tmp = (game -> initialStatus) + i;
        *tmp = (unsigned int*) malloc(width * sizeof(unsigned int));
        memset(*tmp, 0, width * sizeof(unsigned int));
    }
    game -> endStatus = (unsigned int**) malloc(height *sizeof(unsigned int*));
    for (int i = 0; i < height; i++) {
        tmp = (game -> endStatus) + i;
        *tmp = (unsigned int*) malloc(width * sizeof(unsigned int));
        memset(*tmp, 0, sizeof(unsigned int));
    }
    game -> getSize = &(_getSize_);
    game -> nextStep = &(_nextStep_);
    game -> release = &(detachGame);
    
    return game;
}

void detachGame(Game* game) {
    if (!game)
        exit(-1);
    
    for (int i = 0; i < game -> height; i++) {
        free(*(game -> initialStatus + i));
        free(*(game -> endStatus + i));
    }
    free(game -> initialStatus);
    free(game -> endStatus);
    free(game);
}

static unsigned int _getSize_(Game* game) {
    return (unsigned int) (game -> width * game -> height);
}

static void _nextStep_(Game* game) {
    // TODO: Implement game logic
}
