#ifndef INPUT_SEARCH_H
#define INPUT_SEARCH_H

#include "common.h"
#include "pattern_search.h"

char *tolower_string(char *string);
void get_parameters_from_argv(program_arguments *parameters, int argc, char **argv);
int parse_phrase(char *original_string, regex_component **component_list);
void *allocate_dynamic_memory(int num_of_members, int member_size);

#endif