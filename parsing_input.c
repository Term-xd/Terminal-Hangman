#include "parsing_input.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static bool is_input_a_command(char input[128])
{
  trim_leading_and_trailing_whitspaces(input);
  return input[0] == ':';
}

static void print_help(void)
{
  printf("Commands:\n");
  printf("  :help    Show this help text\n");
  printf("  :exit    Quit current game session\n");
  printf("  :hint    Reveal one unrevealed letter (with penalty)\n");
  printf("Guesses: enter a single letter a-z (case-insensitive)\n");
}

static void update_score_from_hints(GameState *game, int letters_revealed_now)
{
  float length = (float)game->word_entry->length -
                 1;

  for (int i = game->revealed_from_hint;
       i < game->revealed_from_hint + letters_revealed_now; i++)
  {
    float fraction = (float)(i + 1) / length;
    float penalty  = 10.0f + ((length * 10.0f) - 10.0f) * fraction * fraction;
    game->score -= (int)penalty;
  }
}

static bool is_fully_revealed(const GameState *game)
{
  for (int i = 0; i < (int)game->word_entry->length; i++)
  {
    if (game->masked[i] == '_')
    {
      return false;
    }
  }
  return true;
}

static void handle_easy_or_medium_hint(GameState *game)
{
  int hidden_count = 0;

  for (int i = 0; i < (int)game->word_entry->length; i++)
  {
    if (game->masked[i] == '_' &&
        isalpha((unsigned char)game->word_entry->word[i]))
    {
      hidden_count++;
    }
  }

  if (hidden_count <= 1)
  {
    printf("\nCan't use hint at the last word.");
    printf("\nPress Enter to continue...");
    discard_rest_of_line();
    return;
  }

  unsigned int target = rand() % hidden_count;

  int random_index = -1;

  for (int i = 0; i < (int)game->word_entry->length; i++)
  {
    if (game->masked[i] == '_' &&
        isalpha((unsigned char)game->word_entry->word[i]))
    {
      if (target == 0)
      {
        random_index = i;
        break;
      }

      target--;
    }
  }

  if (random_index < 0)
  {
    return;
  }

  int  letters_revealed = 0;
  char chosen_letter    = game->word_entry->word[random_index];

  for (int i = 0; i < (int)game->word_entry->length; i++)
  {
    if (game->word_entry->word[i] == chosen_letter)
    {
      game->masked[i] = chosen_letter;
      letters_revealed++;
    }
  }

  game->guessed[chosen_letter - 'a'] = true;
  game->hint_used++;
  update_score_from_hints(game, letters_revealed);
  game->revealed_from_hint += letters_revealed;

  if (is_fully_revealed(game))
  {
    game->status   = STATUS_WON;
    game->ended_at = time(NULL);
  }
}

static void handle_hard_hint(GameState *game)
{
  int hidden_count = 0;

  for (int i = 0; i < (int)game->word_entry->length; i++)
  {
    if (game->masked[i] == '_' &&
        isalpha((unsigned char)game->word_entry->word[i]))
    {
      hidden_count++;
    }
  }

  if (hidden_count <= 1)
  {
    printf("\nCan't use hint at the last word.");
    printf("\nPress Enter to continue...");
    discard_rest_of_line();
    return;
  }

  if (game->hint_used < (int)game->word_entry->cat_count)
  {
    game->hint_used++;
    game->score -= 10;
    return;
  }

  unsigned int target = rand() % hidden_count;

  int random_index = -1;

  for (int i = 0; i < (int)game->word_entry->length; i++)
  {
    if (game->masked[i] == '_' &&
        isalpha((unsigned char)game->word_entry->word[i]))
    {
      if (target == 0)
      {
        random_index = i;
        break;
      }

      target--;
    }
  }

  if (random_index < 0)
  {
    return;
  }

  int  letters_revealed = 0;
  char chosen_letter    = game->word_entry->word[random_index];

  for (int i = 0; i < (int)game->word_entry->length; i++)
  {
    if (game->word_entry->word[i] == chosen_letter)
    {
      game->masked[i] = chosen_letter;
      letters_revealed++;
    }
  }

  game->guessed[chosen_letter - 'a'] = true;
  game->hint_used++;
  update_score_from_hints(game, letters_revealed);
  game->revealed_from_hint += letters_revealed;

  if (is_fully_revealed(game))
  {
    game->status   = STATUS_WON;
    game->ended_at = time(NULL);
  }
}

static void handle_hint(GameState *game)
{
  /* TODO: Wondet if hint limit could be added */

  if (game->difficulty == DIFF_EASY || game->difficulty == DIFF_MEDIUM)
  {
    handle_easy_or_medium_hint(game);
  }
  if (game->difficulty == DIFF_HARD)
  {
    handle_hard_hint(game);
  }
}

static void parse_command(GameState *game, char input[128])
{
  if (strcmp(input, ":help") == 0)
  {
    print_help();
    /* last_guess = 0; */
    printf("\nPress Enter to continue...");
    discard_rest_of_line();
    return;
  }
  else if (strcmp(input, ":exit") == 0)
  {
    game->status   = STATUS_EXIT;
    game->ended_at = time(NULL);
    return;
  }
  else if (strcmp(input, ":hint") == 0)
  {
    handle_hint(game);
    /* last_guess = 0; */
    return;
  }
  else
  {
    printf("Unknown command: %s\n", input);
    /* last_guess = 0; */
    printf("\nPress Enter to continue...");
    discard_rest_of_line();
    return;
  }
}

static bool is_input_a_valid_letter(char input[128])
{
  return isalpha((unsigned char)input[0]) && input[1] == '\0';
}

static bool reveal_letter(GameState *game, char letter)
{
  bool found = false;
  int  n     = (int)game->word_entry->length;
  for (int i = 0; i < n; i++)
  {
    if (tolower(game->word_entry->word[i]) == tolower(letter) &&
        game->masked[i] == '_')
    {
      game->masked[i] = letter;
      found           = true;
    }
  }
  return found;
}

static void check_guess(GameState *game, char letter, char *last_guessed)
{
  if (game->guessed[letter - 'a'])
  {
    printf("Already guessed: %c\n", letter);
    last_guessed[0] = letter;
    printf("\nPress Enter to continue...");
    discard_rest_of_line();
    return;
  }

  /* TODO: Wonder if result of the guess should be displayed
   *       at next render or should not be displayed at all */

  int idx            = letter - 'a';
  game->guessed[idx] = true;
  bool found         = reveal_letter(game, letter);
  if (!found)
  {
    game->wrong_guesses += 1;
    game->score -= 10;
    printf("\nWrong Guess!");
    printf("\nPress Enter to continue...");
    discard_rest_of_line();
    if (game->wrong_guesses >= game->max_wrong)
    {
      game->status   = STATUS_LOST;
      game->ended_at = time(NULL);
    }
  }
  else
  {
    game->score += 20;

    if (is_fully_revealed(game))
    {
      game->status   = STATUS_WON;
      game->ended_at = time(NULL);
    }
    else
    {
      printf("\nCorrect Guess!");
      printf("\nPress Enter to continue...");
      discard_rest_of_line();
    }
  }
  last_guessed[0] = letter;
}

void parse_input(GameState *game, char input[128], char *last_guessed)
{
  if (is_input_a_command(input))
  {
    parse_command(game, input);
    return;
  }
  else if (is_input_a_valid_letter(input))
  {
    check_guess(game, input[0], last_guessed);
  }
  else
  {
    printf("\nInvalid input. Enter a single letter a-z or a command starting "
           "with ':'\n");
    printf("\nPress Enter to continue...");
    discard_rest_of_line();
  }
}
