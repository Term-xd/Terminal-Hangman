#include "leaderboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATS_FILENAME  "hangman_stats.csv"
#define MAX_NAME_LENGTH 128
#define MAX_LINE_LENGTH 512

typedef struct
{
  GameRecord records[MAX_TOP_SCORES];

  int count;

} Leaderboard;

static const char * difficulty_to_string(Difficulty difficulty)
{
  switch (difficulty)
  {
    case DIFF_EASY: return "EASY";

    case DIFF_MEDIUM: return "MEDIUM";

    case DIFF_HARD: return "HARD";

    default: return "UNKNOWN";
  }
}

static const char * status_to_string(GameStatus status)
{
  switch (status)
  {
    case STATUS_WON: return "WON";

    case STATUS_LOST: return "LOST";

    case STATUS_EXIT: return "QUIT";

    default: return "UNKNOWN";
  }
}

static int parse_difficulty(const char * text,
                            Difficulty * result)
{
  if (strcmp(text, "EASY") == 0)
  {
    *result = DIFF_EASY;
    return 1;
  }

  if (strcmp(text, "MEDIUM") == 0)
  {
    *result = DIFF_MEDIUM;
    return 1;
  }

  if (strcmp(text, "HARD") == 0)
  {
    *result = DIFF_HARD;
    return 1;
  }

  return 0;
}

static int parse_status(const char * text, GameStatus * result)
{
  if (strcmp(text, "WON") == 0)
  {
    *result = STATUS_WON;
    return 1;
  }

  if (strcmp(text, "LOST") == 0)
  {
    *result = STATUS_LOST;
    return 1;
  }

  if (strcmp(text, "QUIT") == 0)
  {
    *result = STATUS_EXIT;
    return 1;
  }

  return 0;
}

static void write_csv_string(FILE * fp, const char * text)
{
  fputc('"', fp);

  while (*text != '\0')
  {
    if (*text == '"')
    {
      fputc('"', fp);
      fputc('"', fp);
    }
    else
    {
      fputc(*text, fp);
    }

    text++;
  }

  fputc('"', fp);
}

int save_game_record(const GameRecord * record)
{
  FILE * fp = fopen(STATS_FILENAME, "a");

  if (fp == NULL)
  {
    return 0;
  }

  write_csv_string(fp, record->name);

  fprintf(fp, ",%s,%s,%d,%ld,%d,%d,%ld\n",
          difficulty_to_string(record->difficulty),
          status_to_string(record->status), record->score,
          record->duration, record->hint_used,
          record->wrong_guesses, (long)record->started_at);

  if (ferror(fp))
  {
    fclose(fp);
    return 0;
  }

  fclose(fp);

  return 1;
}

static void remove_line_ending(char * line)
{
  size_t length = strlen(line);

  while (length > 0
         && (line[length - 1] == '\n'
             || line[length - 1] == '\r'))
  {
    line[length - 1] = '\0';

    length--;
  }
}

static int read_csv_field(const char * line, size_t * position,
                          char * output, size_t output_size)
{
  size_t out = 0;

  if (line[*position] == '"')
  {
    (*position)++;

    while (line[*position] != '\0')
    {
      char c = line[*position];

      if (c == '"')
      {
        if (line[*position + 1] == '"')
        {
          if (out + 1 >= output_size)
          {
            return 0;
          }

          output[out++] = '"';

          *position += 2;
        }
        else
        {
          (*position)++;

          if (line[*position] == ',')
          {
            (*position)++;
          }
          else if (line[*position] != '\0')
          {
            return 0;
          }

          output[out] = '\0';

          return 1;
        }
      }
      else
      {
        if (out + 1 >= output_size)
        {
          return 0;
        }

        output[out++] = c;

        (*position)++;
      }
    }

    return 0;
  }

  while (line[*position] != '\0' && line[*position] != ',')
  {
    if (out + 1 >= output_size)
    {
      return 0;
    }

    output[out++] = line[*position];

    (*position)++;
  }

  output[out] = '\0';

  if (line[*position] == ',')
  {
    (*position)++;
  }

  return 1;
}

static int parse_csv_line(const char * line,
                          GameRecord * record)
{
  char fields[8][MAX_LINE_LENGTH];

  size_t position = 0;

  char * endptr;

  long value;

  for (int i = 0; i < 8; i++)
  {
    if (!read_csv_field(line, &position, fields[i],
                        sizeof(fields[i])))
    {
      return 0;
    }
  }

  if (line[position] != '\0')
  {
    return 0;
  }

  if (strlen(fields[0]) >= MAX_NAME_LENGTH)
  {
    return 0;
  }

  strcpy(record->name, fields[0]);

  if (!parse_difficulty(fields[1], &record->difficulty))
  {
    return 0;
  }

  if (!parse_status(fields[2], &record->status))
  {
    return 0;
  }

  value = strtol(fields[3], &endptr, 10);

  if (*endptr != '\0' || value < 0)
  {
    return 0;
  }

  record->score = (int)value;

  value = strtol(fields[4], &endptr, 10);

  if (*endptr != '\0' || value < 0)
  {
    return 0;
  }

  record->duration = value;

  value = strtol(fields[5], &endptr, 10);

  if (*endptr != '\0' || value < 0)
  {
    return 0;
  }

  record->hint_used = (int)value;

  value = strtol(fields[6], &endptr, 10);

  if (*endptr != '\0' || value < 0)
  {
    return 0;
  }

  record->wrong_guesses = (int)value;

  value = strtol(fields[7], &endptr, 10);

  if (*endptr != '\0' || value < 0)
  {
    return 0;
  }

  record->started_at = (time_t)value;

  return 1;
}

static void initialize_leaderboard(Leaderboard * leaderboard)
{
  leaderboard->count = 0;
}

static int
find_insertion_position(const Leaderboard * leaderboard,
                        const GameRecord *  record)
{
  int position = 0;

  while (position < leaderboard->count)
  {
    if (record->score > leaderboard->records[position].score)
    {
      break;
    }

    position++;
  }

  return position;
}

static void insert_into_leaderboard(Leaderboard *      leaderboard,
                                    const GameRecord * record)
{
  int position;

  if (leaderboard->count < MAX_TOP_SCORES)
  {
    position = find_insertion_position(leaderboard, record);

    for (int i = leaderboard->count; i > position; i--)
    {
      leaderboard->records[i] = leaderboard->records[i - 1];
    }

    leaderboard->records[position] = *record;

    leaderboard->count++;

    return;
  }

  if (record->score <= leaderboard->records[9].score)
  {
    return;
  }

  position = find_insertion_position(leaderboard, record);

  for (int i = MAX_TOP_SCORES - 1; i > position; i--)
  {
    leaderboard->records[i] = leaderboard->records[i - 1];
  }

  leaderboard->records[position] = *record;
}

static void build_leaderboard(Leaderboard * leaderboard)
{
  FILE * fp;

  char line[MAX_LINE_LENGTH];

  GameRecord record;

  initialize_leaderboard(leaderboard);

  fp = fopen(STATS_FILENAME, "r");

  if (fp == NULL)
  {
    return;
  }

  while (fgets(line, sizeof(line), fp) != NULL)
  {
    remove_line_ending(line);

    if (line[0] == '\0')
    {
      continue;
    }

    if (!parse_csv_line(line, &record))
    {
      continue;
    }

    insert_into_leaderboard(leaderboard, &record);
  }

  fclose(fp);
}

static void print_leaderboard(Leaderboard * leaderboard)
{
  printf("\n");

  /*
   * Table separator.
   *
   * #          = 4
   * Name       = 26
   * Diff       = 11
   * Score      = 8
   * Duration   = 10
   * H          = 5
   * W          = 5
   * Date       = 21
   */
  const char * separator
      = "+----+--------------------------+-----------+-------"
        "-+----------+-----+-----+---------------------+\n";

  printf("%s", separator);

  printf("|  # |          Name            |   Diff    |  "
         "Score | Duration |  H "
         " |  W  |        Date         |\n");

  printf("%s", separator);

  if (leaderboard->count == 0)
  {
    printf("|    |                          |           |     "
           "   |          |  "
           "   |     |                     |\n");
    printf("%s", separator);

    return;
  }

  for (int i = 0; i < leaderboard->count; i++)
  {
    GameRecord * record = &leaderboard->records[i];

    char name[25];

    size_t name_length = strlen(record->name);

    if (name_length > 24)
    {
      memcpy(name, record->name, 21);
      memcpy(name + 21, "...", 3);
      name[24] = '\0';
    }
    else
    {
      memcpy(name, record->name, name_length + 1);
    }

    char date[32] = "N/A";

    struct tm * local_time = localtime(&record->started_at);

    if (local_time != NULL)
    {
      strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S",
               local_time);
    }

    char duration[16];

    long total_seconds = record->duration;
    long minutes       = total_seconds / 60;
    long seconds       = total_seconds % 60;

    snprintf(duration, sizeof(duration), "%ld:%02ld", minutes,
             seconds);

    printf("| %2d | %-24s | %-9s | %6d | %8s | %3d | %3d | "
           "%-19s |\n",
           i + 1, name,
           difficulty_to_string(record->difficulty),
           record->score, duration, record->hint_used,
           record->wrong_guesses, date);

    printf("%s", separator);
  }
}

void show_leaderboard(void)
{
  Leaderboard leaderboard;

  build_leaderboard(&leaderboard);

  print_leaderboard(&leaderboard);
}
