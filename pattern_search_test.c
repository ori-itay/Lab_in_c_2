#include "input_parser.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ZERO_PARAM_INIT 0
#define INITIAL_LINE_NUM 1
#define CHAR_OFFSET_TO_CURR_LINE_INIT 0
#define LINE_OFFSET_INIT -1
#define LINE_POINTER_INIT 0

void test_search_in_line();

int main()
{
  test_search_in_line();
  return EXIT_SUCCESS;
}

void test_search_in_line()
{
  line line_args = {LINE_POINTER_INIT, INITIAL_LINE_NUM, CHAR_OFFSET_TO_CURR_LINE_INIT, LINE_OFFSET_INIT};
  program_arguments parameters = {ZERO_PARAM_INIT};
  int line_matched_counter = 0, components_count, bytes_read;
  regex_component *components_list;
  char *tested_line = "this should be printed\n";

  line_args.line_ptr = tested_line;
  parameters.phrase = "d be";
  parameters.fp = stdin;
  components_list = (regex_component *)calloc(strlen(parameters.phrase), sizeof(regex_component));
  components_count = parse_phrase(parameters.phrase, &components_list);
  bytes_read = strlen(tested_line);
  search_in_line(&line_args, &parameters, &line_matched_counter, components_list, components_count, bytes_read);
  assert(line_matched_counter == 1);
  line_matched_counter = 0;

  free(components_list);
  components_list = (regex_component *)calloc(strlen(parameters.phrase), sizeof(regex_component));
  tested_line = "shouldn't find a match";
  bytes_read = strlen(tested_line);

  search_in_line(&line_args, &parameters, &line_matched_counter, components_list, components_count, bytes_read);
  assert(line_matched_counter == 0);
  free(components_list);

  tested_line = "1ghello1\n";
  bytes_read = strlen(tested_line);
  parameters.phrase = "shouldn't find a match";
  components_list = (regex_component *)calloc(strlen(parameters.phrase), sizeof(regex_component));
  components_count = parse_phrase(parameters.phrase, &components_list);
  search_in_line(&line_args, &parameters, &line_matched_counter, components_list, components_count, bytes_read);
  assert(line_matched_counter == 0);
  free(components_list);
}