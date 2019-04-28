#include "utilities_for_my_grep.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ACTIVE_ARGUMENT 1
#define DECIMAL_BASE 10
#define HYPHEN_CHAR '-'

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
    } else if (strcmp(argv[index], "-E") == 0) { /*argument -E is further implemented as in original grep*/
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

void* allocate_dynamic_memory(int num_of_members, int member_size){
  void* ret_pointer;

  if ((ret_pointer = calloc(num_of_members, member_size) ) == NULL) {
    printf("Error while allocating memory. exiting...\n");
    exit(EXIT_FAILURE);
  }
  return ret_pointer;
}

void check_getline_error(int bytes_read, regex_component **components_list, program_arguments *parameters,
                         line *line_args)
{
  if (bytes_read == -1 && errno) {
    printf("End of File or error reading a line!\n");
    exit_cleanup(components_list, parameters, line_args);
    exit(EXIT_FAILURE);
  }
}

char *tolower_string(char *string)
{
  int index;
  int length = strlen(string);
  char *lowered_string = (char *)allocate_dynamic_memory(length + 1, sizeof(char));

  strncpy(lowered_string, string, length);
  for (index = 0; index < length; index++) {
    lowered_string[index] = tolower(string[index]);
  }
  return lowered_string;
}

void print_total_match_count(program_arguments *parameters, int line_matched_counter)
{
  if (parameters->c) {
    printf("%d\n", line_matched_counter);
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
