#ifndef SCORE_H_
#define SCORE_H_

#include <SDL2/SDL.h>

#include "chooter.h"

#define HIGH_SCORES_COUNT 8
#define HIGH_SCORE_NAME_LEN_MAX 16

typedef struct {
    bool recent;
    int score;
    char player_name[HIGH_SCORE_NAME_LEN_MAX];
} HighScore;

void high_score_table_init(void);
void high_score_init(State *S);
void add_high_score(int score);

extern HighScore highscores[HIGH_SCORES_COUNT];

#endif // SCORE_H _
