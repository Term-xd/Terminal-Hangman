#include "gameloop.h"
#include "leaderboard.h"
#include "load_data.h"
#include "parsing_input.h"
#include "render.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

/* Exponential Scaling for penalty but Linear for bonus? Why?
 * Because I
 * TODO: Wonder if time bonus can be announced
 */
static void add_time_bonus(GameState *game)
{
  long time_taken = game->ended_at - game->started_at;
  long time_max   = (long)(game->word_entry->length * 20);

  if (time_taken > 0L && time_taken <= time_max)
  {
    float bonus_max = ((float)game->word_entry->length * 10.0f) * 2.0f / 3.0f;
    float bonus_min = 0.0f;
    float fraction  = ((float)time_taken - 1.0f) / ((float)time_max - 1.0f);
    float bonus     = bonus_max - (bonus_max - bonus_min) * fraction;
    game->score += (int)bonus;
  }
}

static void save_game_state(GameState *game, PlayerState *player)
{
  GameRecord game_record;
  strncpy(game_record.name, player->name, sizeof(game_record.name));
  game_record.difficulty    = game->difficulty;
  game_record.hint_used     = game->hint_used;
  game_record.score         = game->score;
  game_record.status        = game->status;
  game_record.started_at    = game->started_at;
  game_record.duration      = game->ended_at - game->started_at;
  game_record.wrong_guesses = game->wrong_guesses;

  if (!save_game_record(&game_record))
  {
    printf("\nCould not save the game state\n");
  }
}

void start_game_loop(PlayerState *player, Difficulty difficulty)
{
  GameState game_state  = {0};
  game_state.difficulty = difficulty;

  load_word_entry(&game_state);
  mask_the_word(&game_state);

  game_state.max_wrong = DEFAULT_MAX_WRONG;
  game_state.status    = STATUS_GUESSING;
  game_state.score     = 100;

  char last_guess = 0;
  char input[128] = {0};

  game_state.started_at = time(NULL);

  while (game_state.status == STATUS_GUESSING)
  {
    render_state(&game_state, player, last_guess);
    printf("Enter a letter or :help :\n> ");

    if (!fgets(input, sizeof(input), stdin))
    {
      continue;
    }
    if (strlen(input) > 126)
    {
      discard_rest_of_line();
    }

    parse_input(&game_state, input, &last_guess);
  }

  render_state(&game_state, player, last_guess);
  if (game_state.status == STATUS_WON)
  {
    add_time_bonus(&game_state);
    if (player->name[0] != '\0')
    {
      printf("Congratulations, %s — you won!\n", player->name);
      save_game_state(&game_state, player);
    }
    else
    {
      printf("Congratulations — you won!\n");
    }
    player->wins += 1;
    printf("Score: %d  |  Time Taken: %ldsec  |  Hints Used: %d  |  Wrong "
           "Guesses: %d\n",
           game_state.score, game_state.ended_at - game_state.started_at,
           game_state.hint_used, game_state.wrong_guesses);
    printf("Press Enter to continue...");
    discard_rest_of_line();
  }
  else if (game_state.status == STATUS_LOST)
  {
    printf("You lost. The word was: %s\n", game_state.word_entry->word);
    player->losses += 1;
    printf("Score: %d  |  Time Taken: %ldsec  |  Hint Used: %d  |  Wrong "
           "Guesses: %d\n",
           game_state.score, game_state.ended_at - game_state.started_at,
           game_state.hint_used, game_state.wrong_guesses);
    printf("Press Enter to continue...");
    discard_rest_of_line();
  }
  /* TODO: Wonder if we should save stats at losing too */
}
