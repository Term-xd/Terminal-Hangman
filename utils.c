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
  printf("\033[2J"); // screen
  printf("\033[H");  // cursor
}

void trim_leading_and_trailing_whitspaces(char string[])
{
  char *start = string;
  while (*start && isspace((unsigned char)*start))
  {
    start++;
  }

  if (*start == '\0')
  {
    *string = '\0';
    return;
  }

  char *end = start;

  while (*end)
  {
    end++;
  }

  end--;

  while (end > start && isspace((unsigned char)*end))
  {
    end--;
  }

  char *dst = string;
  for (char *p = start; p <= end; ++p)
  {
    *dst++ = (char)tolower((unsigned char)*p);
  }
  *dst = '\0';
}
