#include "utils.h"

#include <ctype.h>
#include <stdio.h>

void discard_rest_of_line(void)
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
  {
    /* drop */
  }
}

void clear_screen(void)
{
  /* Portable-ish: print many newlines */
  // for (int i = 0; i < 64; ++i) putchar('\n');
  // printf("\033c"); // reset
  printf("\033[2J");  // screen
  printf("\033[H");   // cursor
}

void trim_leading_and_trailing_whitspaces(char string[])
{
  char * start = string;
  while (*start && isspace((unsigned char)*start))
  {
    start++;
  }

  if (*start == '\0')
  {
    *string = '\0';
    return;
  }

  char * end = start;

  while (*end)
  {
    end++;
  }

  end--;

  while (end > start && isspace((unsigned char)*end))
  {
    end--;
  }

  char * dst = string;
  for (char * p = start; p <= end; ++p)
  {
    *dst++ = (char)tolower((unsigned char)*p);
  }
  *dst = '\0';
}

UserInputResult get_user_input(FILE * stream, char buf[],
                               size_t size)
{
  if (stream == NULL || buf == NULL || size == 0)
  {
    return INPUT_INVALID;
  }

  size_t i = 0;
  int    c;

  while (i + 1 < size)
  {
    c = fgetc(stream);

    if (c == EOF)
    {
      buf[i] = '\0';
      return ferror(stream) ? INPUT_ERROR : INPUT_EOF;
    }

    if (c == '\n')
    {
      buf[i] = '\0';
      return INPUT_OK;
    }

    buf[i++] = (char)c;
  }

  buf[i] = '\0';

  while ((c = fgetc(stream)) != '\n' && c != EOF)
  {
    continue;
  }

  if (c == EOF && ferror(stream))
  {
    return INPUT_ERROR;
  }

  return c == '\n' ? INPUT_TRUNCATED : INPUT_OK;
}
