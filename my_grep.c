#include "my_grep.h"
#include "utilities_for_my_grep.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INCREMENT_1 1
#define BACKSLASH '\\'
#define LEFT_SQUARE_BRACKET '['
#define RIGHT_SQUARE_BRACKET ']'
#define LEFT_ROUND_BRACKET '('
#define RIGHT_ROUND_BRACKET ')'
#define OR_CHAR '|'
#define SPACE_CHAR ' '
#define HYPHEN_CHAR '-'
#define DOT_CHAR '.'
#define ZERO_PARAM_INIT 0
#define INITIAL_LINE_NUM 1
#define LINE_OFFSET_INIT -1
#define STRING_LENGTH_1 1

int   parse_phrase(char *original_string, regex_component **component_list);

void  proccess_line(line *line_args, program_arguments *parameters, int *line_matched_counter,
                   regex_component *components_list, int components_count, int bytes_read);
void  report_line_match(line *line_args, program_arguments *parameters, int *line_matched_counter,
                       regex_component *components_list, int components_count);
int   is_match_in_line(char *haystack, program_arguments *parameters, regex_component *components_list,
                     int components_count);
int   is_match_at_place(char *haystack, int component_index, regex_component *component_list, int component_count,
                      program_arguments *parameters);
int   check_regex_conditions_for_is_match_at_place(regex_component *component_list, int component_index,
                                                 const char *haystack);

int main(int argc, char **argv)
{
  program_arguments parameters = {ZERO_PARAM_INIT};
  line line_args = {ZERO_PARAM_INIT, INITIAL_LINE_NUM, ZERO_PARAM_INIT, LINE_OFFSET_INIT};
  int line_matched_counter = 0, bytes_read, components_count;
  size_t line_len = 0;

  get_parameters_from_argv(&parameters, argc, argv);
  regex_component *components_list = (regex_component *) allocate_dynamic_memory(strlen(parameters.phrase),
                                                                                 sizeof(regex_component));
  components_count = parse_phrase(parameters.phrase, &components_list);
  while ((bytes_read = getline(&(line_args.line_ptr), &line_len, parameters.fp)) > 0) {
    check_getline_error(bytes_read, &components_list, &parameters, &line_args);
    proccess_line(&line_args, &parameters, &line_matched_counter, components_list, components_count, bytes_read);
  }
  print_total_match_count(&parameters, line_matched_counter);
  exit_cleanup(&components_list, &parameters, &line_args);
  return EXIT_SUCCESS;
}

int parse_phrase(char *original_string, regex_component **components_list)
{

  int str_len = strlen(original_string), string_index, component_index = 0;

  for (string_index = 0; string_index < str_len; string_index++) {
    if (original_string[string_index] == DOT_CHAR) {
      (*components_list)[component_index].type = DOT;
    } else if (original_string[string_index] == BACKSLASH) {
      string_index++;
      (*components_list)[component_index].type = ESCAPE_BACKSLASH;
      (*components_list)[component_index].actual_char_to_check = original_string[string_index];
    } else if (original_string[string_index] == LEFT_SQUARE_BRACKET) {
      (*components_list)[component_index].type = SQUARED_BRACKETS;
      while (original_string[++string_index] == SPACE_CHAR) {
      }
      (*components_list)[component_index].low_range_limit = original_string[string_index];
      while (original_string[++string_index] == HYPHEN_CHAR || original_string[string_index] == SPACE_CHAR) {
      }
      (*components_list)[component_index].upper_range_limit = original_string[string_index];
      while (original_string[++string_index] != RIGHT_SQUARE_BRACKET) {
      }
    } else if (original_string[string_index] == LEFT_ROUND_BRACKET) {
      (*components_list)[component_index].type = ROUND_BRACKETS;
      (*components_list)[component_index].start_index_in_phrase = string_index;
      while (original_string[++string_index] != RIGHT_ROUND_BRACKET) {
      }
      (*components_list)[component_index].end_index_in_phrase = string_index;
    } else {
      (*components_list)[component_index].type = REGULAR_CHAR;
      (*components_list)[component_index].actual_char_to_check = original_string[string_index];
    }
    component_index++;
  }
  return component_index;
}

void proccess_line(line *line_args, program_arguments *parameters, int *line_matched_counter,
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
    if(parameters->E){
      match = is_match_at_place(haystack, component_index, components_list, components_count, parameters);
    }
    else if (needle_len == haystack_len - 1) {
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

  int match = 0, current_string_index, compare_length = 0, component_end_index, total_length_compared = 0;

  if (component_index >= component_count) {
    if(parameters->x && strlen(haystack)>STRING_LENGTH_1){
      match = false;
    }
    else{
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