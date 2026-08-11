#ifndef GENERATED_WORDS_H
#define GENERATED_WORDS_H

#include <stddef.h>
#include <stdint.h>


/* Three distinct entry types for different length buckets */
typedef struct
{
  const char *word;
  int         length;
  uint32_t    cat_start;
  uint32_t    cat_count;
} WordEntryShort;

typedef struct
{
  const char *word;
  int         length;
  uint32_t    cat_start;
  uint32_t    cat_count;
} WordEntryMid;

typedef struct
{
  const char *word;
  int         length;
  uint32_t    cat_start;
  uint32_t    cat_count;
} WordEntryLong;

/* Universal */
typedef struct
{
  const char *word;
  int         length;
  uint32_t    cat_start;
  uint32_t    cat_count;
} WordEntry;

/* Global pooled categories */
extern const char  *category_pool[];
extern const size_t category_pool_count;

/* Short group */
extern const uint32_t flat_category_indices_short[];
extern const size_t   flat_index_count_short;
// extern const WordEntryShort words_short[];
extern const WordEntry words_short[];
extern const size_t    words_short_count;

/* Mid group */
extern const uint32_t flat_category_indices_mid[];
extern const size_t   flat_index_count_mid;
// extern const WordEntryMid words_mid[];
extern const WordEntry words_mid[];
extern const size_t    words_mid_count;

/* Long group */
extern const uint32_t flat_category_indices_long[];
extern const size_t   flat_index_count_long;
// extern const WordEntryLong words_long[];
extern const WordEntry words_long[];
extern const size_t    words_long_count;

#endif /* GENERATED_WORDS_H */
