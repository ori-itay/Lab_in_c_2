#include "common.h"
#include "pattern_search.h"


char*  tolower_string(char *string);
void   get_parameters_from_argv(program_arguments *parameters, int argc, char **argv);
int   parse_phrase(char *original_string, regex_component **component_list);
void*  allocate_dynamic_memory(int num_of_members, int member_size);
void exit_cleanup(regex_component **components_list, program_arguments *parameters, line *line_args);
void   check_getline_error(int bytes_read, regex_component **components_list, program_arguments *parameters,
                          line *line_args);