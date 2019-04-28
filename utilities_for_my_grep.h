#ifndef UTILITIES_FOR_MY_GREP_H
#define UTILITIES_FOR_MY_GREP_H

#include "my_grep.h"

void   get_parameters_from_argv(program_arguments *parameters, int argc, char **argv);
void*  allocate_dynamic_memory(int num_of_members, int member_size);
void   check_getline_error(int bytes_read, regex_component **components_list, program_arguments *parameters,
                          line *line_args);
char*  tolower_string(char *string);
void   print_total_match_count(program_arguments *parameters, int line_matched_counter);
void   exit_cleanup(regex_component **components_list, program_arguments *parameters, line *line_args);

#endif