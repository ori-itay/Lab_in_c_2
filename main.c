#include "input_parser.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ZERO_PARAM_INIT 0
#define INITIAL_LINE_NUM 1
#define CHAR_OFFSET_TO_CURR_LINE_INIT 0
#define LINE_OFFSET_INIT -1
#define LINE_POINTER_INIT 0

void find_matches_in_input(line *line_args, program_arguments *parameters, regex_component **components_list,
                           int components_count);
void print_total_match_count(program_arguments *parameters, int line_matched_counter);
void exit_cleanup(regex_component **components_list, program_arguments *parameters, line *line_args);
void check_getline_error(int bytes_read, regex_component **components_list, program_arguments *parameters,
                         line *line_args);

int main(int argc, char **argv)
{
  program_arguments parameters = {ZERO_PARAM_INIT};
  regex_component *components_list;
  line line_args = {LINE_POINTER_INIT, INITIAL_LINE_NUM, CHAR_OFFSET_TO_CURR_LINE_INIT, LINE_OFFSET_INIT};
  int components_count;

  get_parameters_from_argv(&parameters, argc, argv);
  components_list = (regex_component *)calloc(strlen(parameters.phrase), sizeof(regex_component));
  components_count = parse_phrase(parameters.phrase, &components_list);
  find_matches_in_input(&line_args, &parameters, &components_list, components_count);
  exit_cleanup(&components_list, &parameters, &line_args);
  return EXIT_SUCCESS;
}

void find_matches_in_input(line *line_args, program_arguments *parameters, regex_component **components_list,
                           int components_count)
{
  int bytes_read, line_matched_counter = 0;
  size_t line_len = 0;

  while ((bytes_read = getline(&(line_args->line_ptr), &line_len, parameters->fp)) > 0) {
    check_getline_error(bytes_read, components_list, parameters, line_args);
    search_in_line(line_args, parameters, &line_matched_counter, *components_list, components_count, bytes_read);
  }
  print_total_match_count(parameters, line_matched_counter);
}

void print_total_match_count(program_arguments *parameters, int line_matched_counter)
{
  if (parameters->c) {
    printf("%d\n", line_matched_counter);
  }
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
