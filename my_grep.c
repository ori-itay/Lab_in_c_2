#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define INCREMENT_1 1
#define DECIMAL_BASE 10
#define BACKSLASH '\\'
#define LEFT_SQUARE_BRACKET '['
#define RIGHT_SQUARE_BRACKET ']'
#define LEFT_ROUND_BRACKET '('
#define RIGHT_ROUND_BRACKET ')'
#define OR_CHAR '|'
#define HYPHEN_CHAR '-'
#define SPACE_CHAR ' '

typedef struct program_arguments {
  int A, NUM, b, c, i, n, v, x, E;
  char *phrase;
  FILE *fp;
} program_arguments;

typedef struct line {
  char *line_ptr;
  int current_line_num, char_offset, line_offset_since_match;
} line;

typedef enum regex_type { REGULAR_CHAR, ESCAPE_BACKSLASH, DOT, SQUARED_BRACKETS, ROUND_BRACKETS } regex_type;

typedef struct regex_component {
  regex_type type;
  int low_range_limit, upper_range_limit, start_index_in_phrase, end_index_in_phrase;
  char actual_char_to_check;
} regex_component;

void get_parameters_from_argv(program_arguments *parameters, int argc, char **argv);
int parse_phrase(char *original_string, regex_component **component_list);
char *tolower_string(char *string);
void check_getline_error(int bytes_read, regex_component **components_list, program_arguments *parameters,
                         line *line_args);
void proccess_line(line *line_args, program_arguments *parameters, int *line_matched_count,
                   regex_component *components_list, int components_count, char **current_line, int bytes_read);
void report_line_match(line *line_args, program_arguments *parameters, int *line_matched_count,
                       regex_component *components_list, int components_count); /* create another module of regex? */
int is_match_in_line(char *haystack, program_arguments *parameters, regex_component *components_list,
                     int components_count);
int is_match_at_place(char *haystack, int component_index, regex_component *component_list, int component_count,
                      program_arguments *parameters);
int check_regex_conditions_for_is_match_at_place(regex_component *component_list, int component_index,
                                                 const char *haystack);
void print_total_match_count(program_arguments *parameters, int line_matched_count);
void exit_cleanup(regex_component **components_list, program_arguments *parameters, line *line_args);

int main(int argc, char **argv)
{
  program_arguments parameters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL};
  line line_args = {0, 1, 0, -1};
  int line_matched_count = 0, bytes_read, components_count = 0;
  char *current_line = NULL;
  size_t line_len = 0;
  regex_component *components_list;

  get_parameters_from_argv(&parameters, argc, argv);
  if ((components_list = (regex_component *)calloc(strlen(parameters.phrase), sizeof(regex_component))) == NULL) {
    printf("Error while allocating memory. exiting...\n");
    exit(1);
  }

  components_count = parse_phrase(parameters.phrase, &components_list);
  while ((bytes_read = getline(&current_line, &line_len, parameters.fp)) > 0) {
    check_getline_error(bytes_read, &components_list, &parameters, &line_args);
    proccess_line(&line_args, &parameters, &line_matched_count, components_list, components_count, &current_line,
                  bytes_read);
  }
  print_total_match_count(&parameters, line_matched_count);
  exit_cleanup(&components_list, &parameters, &line_args);
  return 0;
}

void get_parameters_from_argv(program_arguments *parameters, int argc, char **argv)
{
  int index;

  if (argc < 2) {
    printf("Error: wrong number of arguments. Exiting...\n");
    exit(1);
  }

  for (index = 1; index < argc; index++) {
    if (argv[index][0] != '-' && parameters->phrase == NULL) {
      parameters->phrase = argv[index];
    } else if (index == argc - 1 && argv[index][0] != '-' && parameters->phrase != NULL) {
      if ((parameters->fp = fopen(argv[argc - 1], "r")) == NULL) {
        printf("Error while opening file. Exiting...\n");
        exit(1);
      }
    } else if (strcmp(argv[index], "-A") == 0) {
      parameters->A = 1;
      parameters->NUM = (int)strtoul(argv[++index], NULL, DECIMAL_BASE);
    } else if (strcmp(argv[index], "-b") == 0) {
      parameters->b = 1;
    } else if (strcmp(argv[index], "-c") == 0) {
      parameters->c = 1;
    } else if (strcmp(argv[index], "-i") == 0) {
      parameters->i = 1;
    } else if (strcmp(argv[index], "-n") == 0) {
      parameters->n = 1;
    } else if (strcmp(argv[index], "-v") == 0) {
      parameters->v = 1;
    } else if (strcmp(argv[index], "-x") == 0) {
      parameters->x = 1;
    } else if (strcmp(argv[index], "-E") == 0) {
      parameters->E = 1;
    }
  }
  if (parameters->fp == NULL) {
    parameters->fp = stdin;
  }
  if (parameters->i) {
    parameters->phrase = tolower_string(parameters->phrase);
  }
  return;
}

int parse_phrase(char *original_string, regex_component **components_list)
{

  int str_len = strlen(original_string), string_index, component_index = 0;

  for (string_index = 0; string_index < str_len; string_index++) {
    if (original_string[string_index] == '.') {
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

char *tolower_string(char *string)
{
  int index;
  int length = strlen(string);
  char *lowered_string = (char *)calloc(length + 1, sizeof(char));
  if (lowered_string == NULL) {
    printf("Error while allocating memory. exiting...\n");
    exit(1);
  }
  strncpy(lowered_string, string, length);
  for (index = 0; index < length; index++) {
    lowered_string[index] = tolower(string[index]);
  }
  return lowered_string;
}

void check_getline_error(int bytes_read, regex_component **components_list, program_arguments *parameters,
                         line *line_args)
{
  if (bytes_read == -1 && errno) {
    printf("End of File or error reading a line!\n");
    exit_cleanup(components_list, parameters, line_args);
    exit(1);
  }
}

void proccess_line(line *line_args, program_arguments *parameters, int *line_matched_count,
                   regex_component *components_list, int components_count, char **current_line, int bytes_read)
{

  line_args->line_ptr = *current_line;
  report_line_match(line_args, parameters, line_matched_count, components_list, components_count);
  line_args->current_line_num++;
  line_args->char_offset += bytes_read;
}

void report_line_match(line *line_args, program_arguments *parameters, int *line_matched_count,
                       regex_component *components_list, int components_count)
{

  int match = 0;
  char *haystack = line_args->line_ptr;

  if ((match = is_match_in_line(haystack, parameters, components_list, components_count))) {

    if (line_args->line_offset_since_match == 0) {
      printf("--\n");
    }
    (*line_matched_count)++;
    if (parameters->c) {
      return;
    }
    if (parameters->n) {
      printf("%d:", line_args->current_line_num);
    }
    if (parameters->b) {
      printf("%d:", line_args->char_offset);
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
      printf("%d-", line_args->char_offset);
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
    if (needle_len == haystack_len - 1) {
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
    match = TRUE;
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
      compare_length = 0;
      if (match) {
        break;
      }
    }
  } else {
    match = FALSE;
  }
  return match;
}

int check_regex_conditions_for_is_match_at_place(regex_component *component_list, int component_index,
                                                 const char *haystack)
{

  if ((component_list[component_index].type == REGULAR_CHAR ||
       component_list[component_index].type == ESCAPE_BACKSLASH) &&
      component_list[component_index].actual_char_to_check == haystack[0]) {
    return TRUE;
  }
  if (component_list[component_index].type == DOT) {
    return TRUE;
  }
  if (component_list[component_index].type == SQUARED_BRACKETS &&
      component_list[component_index].low_range_limit <= haystack[0] &&
      component_list[component_index].upper_range_limit >= haystack[0]) {
    return TRUE;
  }
  return FALSE;
}

void print_total_match_count(program_arguments *parameters, int line_matched_count)
{
  if (parameters->c) {
    printf("%d\n", line_matched_count);
  }
}

void exit_cleanup(regex_component **components_list, program_arguments *parameters, line *line_args)
{

  if (parameters->i) {
    free(parameters->phrase);
  }
  if (line_args->line_ptr != NULL) {
    free(line_args->line_ptr);
  }
  free(*components_list);
  fclose(parameters->fp);
}