#include "input_parser.h"


#include <stdlib.h>
#include <string.h>

#define ZERO_PARAM_INIT 0
#define INITIAL_LINE_NUM 1
#define LINE_OFFSET_INIT -1




void print_total_match_count(program_arguments *parameters, int line_matched_counter);


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
  while ((bytes_read = getline(&(line_args.line_ptr), &line_len, parameters.fp)) > 0) { // Ori, get these 3 lines into one function?
    check_getline_error(bytes_read, &components_list, &parameters, &line_args); // Ori, get these 3 lines into one function?
    proccess_line(&line_args, &parameters, &line_matched_counter, components_list, components_count, bytes_read);   // Ori, get these 3 lines into one function?
  }
  print_total_match_count(&parameters, line_matched_counter);
  exit_cleanup(&components_list, &parameters, &line_args);
  return EXIT_SUCCESS;
}


void print_total_match_count(program_arguments *parameters, int line_matched_counter)
{
  if (parameters->c) {
    printf("%d\n", line_matched_counter);
  }
}


