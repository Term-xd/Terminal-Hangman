#ifndef DATA_H
#define DATA_H

#include <stdbool.h>
#include <time.h>

#include "generated_words.h"

/* TODO: Wonder if 128 of `input[128]` should be a macro */
#define MAX_WORD_LEN 64 // TODO: This should be provided by generated_words.h
#define DEFAULT_MAX_WRONG 6

typedef enum
{
  STATUS_GUESSING,
  STATUS_WON,
  STATUS_LOST,
  STATUS_EXIT
} GameStatus;

typedef enum
{
  DIFF_EASY,
  DIFF_MEDIUM,
  DIFF_HARD
} Difficulty;

typedef struct
{
  const WordEntry *word_entry;
  char             masked[MAX_WORD_LEN];
  bool             guessed[26];
  int              wrong_guesses;
  int              max_wrong;
  GameStatus       status;
  Difficulty       difficulty;
  time_t           started_at;
  time_t           ended_at;
  int              hint_used;
  int              revealed_from_hint;
  int              score;
} GameState;

typedef struct
{
  char name[64];
  int  wins;
  int  losses;
} PlayerState;

#endif // !DATA_H
