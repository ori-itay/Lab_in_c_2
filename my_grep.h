#ifndef MY_GREP_H
#define MY_GREP_H

#include <stdio.h>

typedef struct program_arguments {
  int A, NUM, b, c, i, n, v, x, E;
  char *phrase;
  FILE *fp;
} program_arguments;

typedef struct line {
  char *line_ptr;
  int current_line_num, char_offset_to_curr_line, line_offset_since_match;
} line;

typedef enum regex_type { REGULAR_CHAR, ESCAPE_BACKSLASH, DOT, SQUARED_BRACKETS, ROUND_BRACKETS } regex_type;

typedef struct regex_component {
  regex_type type;
  int low_range_limit, upper_range_limit, start_index_in_phrase, end_index_in_phrase;
  char actual_char_to_check;
} regex_component;

#endif
