#include "data.h"
#include "gameloop.h"
#include "leaderboard.h"
#include "utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void show_menu(void)
{
  printf("=== Hangman (Terminal) ===\n");
  printf("1) Play (Easy)\n");
  printf("2) Play (Medium)\n");
  printf("3) Play (Hard)\n");
  printf("4) Show Session History\n");
  printf("5) Show Leaderboard\n");
  printf("6) Quit\n");
}

int main(void)
{
  srand((unsigned int)time(NULL));

  clear_screen();

  PlayerState player = {.name = "", .wins = 0, .losses = 0};

  printf("Enter your name or press Enter to play as Guest: ");
  char name[64];
  if (fgets(name, sizeof(name), stdin))
  {
    trim_leading_and_trailing_whitspaces(name);
    if (name[0] != '\0')
    {
      strncpy(player.name, name, sizeof(player.name) - 1);
    }
  }

  if (strlen(name) > 62)
  {
    discard_rest_of_line();
  }

  clear_screen();

  bool keep_playing = true;
  int  option;
  while (keep_playing)
  {
    show_menu();

    printf("Choose an option: ");
    option = getchar();
    discard_rest_of_line();
    switch (option)
    {
      case '1': start_game_loop(&player, DIFF_EASY);
                break;
      case '2': start_game_loop(&player, DIFF_MEDIUM);
                break;
      case '3': start_game_loop(&player, DIFF_HARD);
                break;
      case '4': // show_session_history();
                printf("Not implemented yet. But it is easy. So feel free to make a PR.");
                break;
      case '5': show_leaderboard();
                printf("Press Enter to continue...");
                discard_rest_of_line();
                break;
      case '6': keep_playing = false;
                break;
      default:  printf("\nInvalid Choice\n");
                printf("Press Enter to continue...");
                discard_rest_of_line();
                break;
    }
    clear_screen();
  }

  printf("Thanks for playing");
  if (player.name[0] != '\0')
  {
    printf(", %s", player.name);
  }
  printf(". Wins: %d Losses: %d\n", player.wins, player.losses);

  return EXIT_SUCCESS;
}
