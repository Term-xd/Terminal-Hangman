#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <time.h>

#include "data.h"

#define MAX_TOP_SCORES 10

typedef struct
{
  char name[128];

  Difficulty difficulty;
  GameStatus status;

  int score;

  long duration;
  int  hint_used;
  int  wrong_guesses;

  time_t started_at;

} GameRecord;

int save_game_record(const GameRecord * record);

void show_leaderboard(void);

#endif  // LEADERBOARD_H
