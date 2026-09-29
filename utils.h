#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>

typedef enum
{
  INPUT_OK,
  INPUT_TRUNCATED,
  INPUT_EOF,
  INPUT_ERROR,
  INPUT_INVALID
} UserInputResult;

void clear_screen(void);

void trim_leading_and_trailing_whitspaces(char string[]);

UserInputResult get_user_input(FILE * stream, char buf[],
                               size_t size);
void            discard_rest_of_line(void);

#endif  // !UTILS_H
