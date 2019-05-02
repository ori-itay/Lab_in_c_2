#include "input_parser.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define BACKSLASH '\\'
#define LEFT_SQUARE_BRACKET '['
#define RIGHT_SQUARE_BRACKET ']'
#define LEFT_ROUND_BRACKET '('
#define RIGHT_ROUND_BRACKET ')'
#define SPACE_CHAR ' '
#define HYPHEN_CHAR '-'
#define DOT_CHAR '.'
#define HYPHEN_CHAR '-'

#define ACTIVE_ARGUMENT 1
#define DECIMAL_BASE 10

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

char *tolower_string(char *string)
{
  int index;
  int length = strlen(string);
  char *lowered_string = (char *)calloc(length + 1, sizeof(char));

  strncpy(lowered_string, string, length);
  for (index = 0; index < length; index++) {
    lowered_string[index] = tolower(string[index]);
  }
  return lowered_string;
}

void get_parameters_from_argv(program_arguments *parameters, int argc, char **argv)
{
  int index;

  if (argc < 2) {
    printf("Error: wrong number of arguments. Exiting...\n");
    exit(EXIT_FAILURE);
  }

  for (index = 1; index < argc; index++) {
    if (argv[index][0] != HYPHEN_CHAR && parameters->phrase == NULL) {
      parameters->phrase = argv[index];
    } else if (index == argc - 1 && argv[index][0] != HYPHEN_CHAR && parameters->phrase != NULL) {
      if ((parameters->fp = fopen(argv[argc - 1], "r")) == NULL) {
        printf("Error while opening file. Exiting...\n");
        exit(EXIT_FAILURE);
      }
    } else if (strcmp(argv[index], "-A") == 0) {
      parameters->A = ACTIVE_ARGUMENT;
      parameters->NUM = (int)strtoul(argv[++index], NULL, DECIMAL_BASE);
    } else if (strcmp(argv[index], "-b") == 0) {
      parameters->b = ACTIVE_ARGUMENT;
    } else if (strcmp(argv[index], "-c") == 0) {
      parameters->c = ACTIVE_ARGUMENT;
    } else if (strcmp(argv[index], "-i") == 0) {
      parameters->i = ACTIVE_ARGUMENT;
    } else if (strcmp(argv[index], "-n") == 0) {
      parameters->n = ACTIVE_ARGUMENT;
    } else if (strcmp(argv[index], "-v") == 0) {
      parameters->v = ACTIVE_ARGUMENT;
    } else if (strcmp(argv[index], "-x") == 0) {
      parameters->x = ACTIVE_ARGUMENT;
    } else if (strcmp(argv[index], "-E") == 0) {
      parameters->E = ACTIVE_ARGUMENT;
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

