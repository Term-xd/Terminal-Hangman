#include "render.h"
#include "generated_words.h"
#include "utils.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

static const char * HANGMAN_STAGES[] = {
  /* 0 */
  "  +---+\n"
  "  |   |\n"
  "      |\n"
  "      |\n"
  "      |\n"
  "      |\n"
  "=========\n",
  /* 1 */
  "  +---+\n"
  "  |   |\n"
  "  O   |\n"
  "      |\n"
  "      |\n"
  "      |\n"
  "=========\n",
  /* 2 */
  "  +---+\n"
  "  |   |\n"
  "  O   |\n"
  "  |   |\n"
  "      |\n"
  "      |\n"
  "=========\n",
  /* 3 */
  "  +---+\n"
  "  |   |\n"
  "  O   |\n"
  " /|   |\n"
  "      |\n"
  "      |\n"
  "=========\n",
  /* 4 */
  "  +---+\n"
  "  |   |\n"
  "  O   |\n"
  " /|\\  |\n"
  "      |\n"
  "      |\n"
  "=========\n",
  /* 5 */
  "  +---+\n"
  "  |   |\n"
  "  O   |\n"
  " /|\\  |\n"
  " /    |\n"
  "      |\n"
  "=========\n",
  /* 6 */
  "  +---+\n"
  "  |   |\n"
  "  O   |\n"
  " /|\\  |\n"
  " / \\  |\n"
  "      |\n"
  "=========\n"
};

static void render_ascii_hangman(int wrong, int max_wrong)
{
  int idx = wrong;
  if (idx < 0)
  {
    idx = 0;
  }
  if (idx > max_wrong)
  {
    idx = max_wrong;
  }
  if (idx > (int)(sizeof(HANGMAN_STAGES)
                      / sizeof(HANGMAN_STAGES[0])
                  - 1))
  {
    idx = (int)(sizeof(HANGMAN_STAGES)
                    / sizeof(HANGMAN_STAGES[0])
                - 1);
  }
  printf("%s", HANGMAN_STAGES[idx]);
}

static void render_masked_word(const char * masked)
{
  printf("Word to guess: ");
  for (size_t i = 0; i < strlen(masked); ++i)
  {
    printf("%c ", toupper(masked[i]));
  }
  printf("\n");
}

static void render_guessed_letters(const bool guessed[26],
                                   char       last_guess)
{
  bool first = true;
  printf("Guessed: ");
  for (int i = 0; i < 26; i++)
  {
    if (guessed[i])
    {
      if (!first)
      {
        printf(", ");
      }
      printf("%c", 'A' + i);
      first = false;
    }
  }
  if (first)
  {
    printf("(none)");
  }
  if (last_guess)
  {
    printf("   (last: %c)", toupper(last_guess));
  }
  printf("\n");
}

static void render_hard_category(const GameState * game)
{
  if (game->hint_used == 0)
  {
    printf("\n");
    return;
  }

  printf("%s: ", (game->word_entry->cat_count > 1)
                     ? "Categories"
                     : "Category");

  if (game->hint_used >= (int)game->word_entry->cat_count)
  {
    for (int i = 0; i < (int)game->word_entry->cat_count; i++)
    {
      if (i)
      {
        printf(", ");
      }
      uint32_t index = flat_category_indices_long
          [game->word_entry->cat_start + i];
      printf("%s", category_pool[index]);
    }
    printf("\n\n");
    return;
  }

  for (int i = 0; i < game->hint_used; i++)
  {
    if (i)
    {
      printf(", ");
    }
    uint32_t index = flat_category_indices_long
        [game->word_entry->cat_start + i];
    printf("%s", category_pool[index]);
  }

  printf(", ...\n\n");
}

static void
render_difficulty_and_category(const GameState * game)
{
  if (game->difficulty == DIFF_EASY)
  {
    printf("Difficulty: Easy\n");

    printf("%s: ", (game->word_entry->cat_count > 1)
                       ? "Categories"
                       : "Category");
    for (uint32_t i = 0; i < game->word_entry->cat_count; i++)
    {
      uint32_t index = flat_category_indices_short
          [game->word_entry->cat_start + i];
      printf("%s%s", category_pool[index],
             (i == (game->word_entry->cat_count - 1)) ? ""
                                                      : ", ");
    }
    printf("\n\n");
  }

  if (game->difficulty == DIFF_MEDIUM)
  {
    printf("Difficulty: Medium\n");

    printf("%s: ", (game->word_entry->cat_count > 1)
                       ? "Categories"
                       : "Category");
    for (uint32_t i = 0; i < game->word_entry->cat_count; i++)
    {
      uint32_t index = flat_category_indices_mid
          [game->word_entry->cat_start + i];
      printf("%s%s\n\n", category_pool[index],
             (i == (game->word_entry->cat_count - 1)) ? ""
                                                      : ", ");
    }
    printf("\n\n");
  }

  if (game->difficulty == DIFF_HARD)
  {
    printf("Difficulty: Hard\n");

    render_hard_category(game);
  }
}

static void render_header(const PlayerState * player)
{
  if (player && player->name[0] != '\0')
  {
    printf("Player: %s   Wins: %d   Losses: %d\n\n",
           player->name, player->wins, player->losses);
  }
  else
  {
    printf("Player: Guest   Wins: %d   Losses: %d\n\n",
           player->wins, player->losses);
  }
}

void render_state(const GameState *   game,
                  const PlayerState * player,
                  const char          last_guess)
{
  clear_screen();

  render_header(player);

  render_ascii_hangman(game->wrong_guesses, game->max_wrong);

  render_difficulty_and_category(game);

  render_masked_word(game->masked);
  render_guessed_letters(game->guessed, last_guess);
  printf("Attempts left: %d\n\n",
         game->max_wrong - game->wrong_guesses);

  printf("Score: %d\n\n", game->score);
}
