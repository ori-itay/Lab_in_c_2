#include "input_parser.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OR_CHAR '|'
#define STRING_LENGTH_1 1
#define INCREMENT_1 1

void report_line_match(line *line_args, program_arguments *parameters, int *line_matched_counter,
                       regex_component *components_list, int components_count);
int is_match_in_line(char *haystack, program_arguments *parameters, regex_component *components_list,
                     int components_count);
int is_match_at_place(char *haystack, int component_index, regex_component *component_list, int component_count,
                      program_arguments *parameters);
int check_regex_conditions_for_is_match_at_place(regex_component *component_list, int component_index,
                                                 const char *haystack);

void search_in_line(line *line_args, program_arguments *parameters, int *line_matched_counter,
                    regex_component *components_list, int components_count, int bytes_read)
{
  report_line_match(line_args, parameters, line_matched_counter, components_list, components_count);
  line_args->current_line_num++;
  line_args->char_offset_to_curr_line += bytes_read;
}

void report_line_match(line *line_args, program_arguments *parameters, int *line_matched_counter,
                       regex_component *components_list, int components_count)
{

  int match = 0;
  char *haystack = line_args->line_ptr;

  if ((match = is_match_in_line(haystack, parameters, components_list, components_count))) {

    if (line_args->line_offset_since_match == 0) {
      printf("--\n");
    }
    (*line_matched_counter)++;
    if (parameters->c) {
      return;
    }
    if (parameters->n) {
      printf("%d:", line_args->current_line_num);
    }
    if (parameters->b) {
      printf("%d:", line_args->char_offset_to_curr_line);
    }
    printf("%s", haystack);

    if (parameters->A) {
      line_args->line_offset_since_match = parameters->NUM;
    }

  } else if (parameters->A && line_args->line_offset_since_match > 0) {
    if (parameters->n) {
      printf("%d-", line_args->current_line_num);
    }
    if (parameters->b) {
      printf("%d-", line_args->char_offset_to_curr_line);
    }
    printf("%s", haystack);
    if (line_args->line_offset_since_match > 0) {
      (line_args->line_offset_since_match)--;
    }
  }
}

int is_match_in_line(char *haystack, program_arguments *parameters, regex_component *components_list,
                     int components_count)
{
  int match = 0, needle_len = strlen(parameters->phrase), haystack_len = strlen(haystack), component_index = 0,
      haystack_index = 0;

  if (parameters->i) {
    haystack = tolower_string(haystack);
  }
  if (parameters->x) {
    if (parameters->E) {
      match = is_match_at_place(haystack, component_index, components_list, components_count, parameters);
    } else if (needle_len == haystack_len - 1) {
      match = is_match_at_place(haystack, component_index, components_list, components_count, parameters);
    }
  } else {
    for (; haystack_index < haystack_len; haystack_index++) {
      if ((match = is_match_at_place(haystack + haystack_index, component_index, components_list, components_count,
                                     parameters))) {
        break;
      }
    }
  }
  match ^= parameters->v; /* invert result if -v flag is entered */

  if (parameters->i) {
    free(haystack);
  }
  return match;
}

int is_match_at_place(char *haystack, int component_index, regex_component *component_list, int component_count,
                      program_arguments *parameters)
{

  int match = 0, current_string_index, compare_length = 0, component_end_index;

  if (component_index >= component_count) {
    if (parameters->x && strlen(haystack) > STRING_LENGTH_1) {
      match = false;
    } else {
      match = true;
    }
  } else if (check_regex_conditions_for_is_match_at_place(component_list, component_index, haystack)) {
    match = is_match_at_place(haystack + INCREMENT_1, component_index + INCREMENT_1, component_list, component_count,
                              parameters);
  } else if (component_list[component_index].type == ROUND_BRACKETS) {
    current_string_index = component_list[component_index].start_index_in_phrase;
    component_end_index = component_list[component_index].end_index_in_phrase;
    while (current_string_index < component_end_index) {
      while (parameters->phrase[++current_string_index] != OR_CHAR && current_string_index < component_end_index) {
        compare_length++;
      }
      if (strncmp(parameters->phrase + current_string_index - compare_length, haystack, compare_length) == 0) {
        match = is_match_at_place(haystack + compare_length, component_index + 1, component_list, component_count,
                                  parameters);
      }
      if (match) {
        break;
      }
      compare_length = 0;
    }
  } else {
    match = false;
  }
  return match;
}

int check_regex_conditions_for_is_match_at_place(regex_component *component_list, int component_index,
                                                 const char *haystack)
{

  if ((component_list[component_index].type == REGULAR_CHAR ||
       component_list[component_index].type == ESCAPE_BACKSLASH) &&
      component_list[component_index].actual_char_to_check == haystack[0]) {
    return true;
  }
  if (component_list[component_index].type == DOT) {
    return true;
  }
  if (component_list[component_index].type == SQUARED_BRACKETS &&
      component_list[component_index].low_range_limit <= haystack[0] &&
      component_list[component_index].upper_range_limit >= haystack[0]) {
    return true;
  }
  return false;
}