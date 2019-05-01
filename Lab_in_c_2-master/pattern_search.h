#ifndef PATTERN_SEARCH_H
#define PATTERN_SEARCH_H

#include "common.h"

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

void search_in_line(line *line_args, program_arguments *parameters, int *line_matched_counter,
                    regex_component *components_list, int components_count, int bytes_read);

#endif
