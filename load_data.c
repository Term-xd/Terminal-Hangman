#include "load_data.h"
#include <ctype.h>
#include <stdlib.h>

static size_t pick_random_index(size_t count)
{
  return (size_t)(rand() % (int)count);
}

void load_word_entry(GameState * state)
{
  size_t index;
  switch (state->difficulty)
  {
    case DIFF_EASY:
      index             = pick_random_index(words_short_count);
      state->word_entry = &words_short[index];
      break;

    case DIFF_MEDIUM:
      index             = pick_random_index(words_mid_count);
      state->word_entry = &words_mid[index];
      break;

    case DIFF_HARD:
      index             = pick_random_index(words_long_count);
      state->word_entry = &words_long[index];
      break;

    default: break;
  }
}

void mask_the_word(GameState * game)
{
  int length = game->word_entry->length;
  for (int i = 0; i < length; ++i)
  {
    if (isalpha((unsigned char)game->word_entry->word[i]))
    {
      game->masked[i] = '_';
    }
    else
    {
      game->masked[i] = game->word_entry->word[i];
    }
  }
  game->masked[length] = '\0';
}
