#include "input_parser.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void test_get_parameters_from_argv();
void test_allocate_dynamic_memory();
void test_tolower_string();
void test_parse_phrase();

#define NUMBER_OF_ARGS 5
#define IS_SET 1
#define IS_NOT_SET 0
#define ZERO_PARAM_INIT 0

#define INDEX_OF_DOT_TEST_ONE 5
#define MAX_INDEX_TEST_ONE 11
#define RESULT_NUM_OF_COMPONENTS_TEST_ONE 12

#define RESULT_NUM_OF_COMPONENTS_TEST_TWO 12

#define RESULT_NUM_OF_COMPONENTS_TEST_THREE 5
#define RESULT_INDEX_OF_BACKSLASH 4

#define RESULT_NUM_OF_COMPONENTS_TEST_FOUR 2

int main()
{
  test_get_parameters_from_argv();
  test_tolower_string();
  test_allocate_dynamic_memory();
  test_parse_phrase();
  return 0;
}

void test_parse_phrase()
{
  int index;
  char *phrase = "hello.world[!-  $]";
  regex_component *components_list =
      (regex_component *)allocate_dynamic_memory(strlen(phrase), sizeof(regex_component));
  int tested_num_of_components = parse_phrase(phrase, &components_list);
  assert(tested_num_of_components == RESULT_NUM_OF_COMPONENTS_TEST_ONE);
  for (index = 0; index < MAX_INDEX_TEST_ONE; index++) {
    if (index != INDEX_OF_DOT_TEST_ONE) {
      assert(components_list[index].type == REGULAR_CHAR);
    } else {
      assert(components_list[INDEX_OF_DOT_TEST_ONE].type == DOT);
    }
    if (index != INDEX_OF_DOT_TEST_ONE) {
      assert(components_list[index].actual_char_to_check == phrase[index]);
    }
  }
  assert(components_list[MAX_INDEX_TEST_ONE].type == SQUARED_BRACKETS);
  assert(components_list[index].low_range_limit == '!' && components_list[index].upper_range_limit == '$');
  free(components_list);

  phrase = "(hello|world)";
  components_list = (regex_component *)allocate_dynamic_memory(strlen(phrase), sizeof(regex_component));
  tested_num_of_components = parse_phrase(phrase, &components_list);
  assert(tested_num_of_components == 1);
  assert(components_list[0].type == ROUND_BRACKETS && components_list[0].start_index_in_phrase == 0);
  assert(components_list[0].end_index_in_phrase == RESULT_NUM_OF_COMPONENTS_TEST_TWO);
  free(components_list);

  phrase = "\\[1-2\\]";
  components_list = (regex_component *)allocate_dynamic_memory(strlen(phrase), sizeof(regex_component));
  tested_num_of_components = parse_phrase(phrase, &components_list);
  assert(tested_num_of_components == RESULT_NUM_OF_COMPONENTS_TEST_THREE);
  assert(components_list[0].type == ESCAPE_BACKSLASH);
  assert(components_list[RESULT_INDEX_OF_BACKSLASH].type == ESCAPE_BACKSLASH);
  for (index = 1; index < tested_num_of_components - 1; index++) {
    assert(components_list[index].type == REGULAR_CHAR);
  }
  free(components_list);

  phrase = "(a | )[b-d]";
  components_list = (regex_component *)allocate_dynamic_memory(strlen(phrase), sizeof(regex_component));
  tested_num_of_components = parse_phrase(phrase, &components_list);
  assert(tested_num_of_components == RESULT_NUM_OF_COMPONENTS_TEST_FOUR);
  assert(components_list[0].type == ROUND_BRACKETS);
  assert(components_list[1].type == SQUARED_BRACKETS);
  assert(components_list[1].low_range_limit == 'b');
  assert(components_list[1].upper_range_limit == 'd');
  free(components_list);
}

void test_get_parameters_from_argv()
{
  program_arguments tested_paramters = {ZERO_PARAM_INIT};
  char *argv[NUMBER_OF_ARGS] = {"search_me", "-A", "-i", "-v", ""};
  int argc = NUMBER_OF_ARGS;
  get_parameters_from_argv(&tested_paramters, argc, argv);
  assert(tested_paramters.A == IS_SET || tested_paramters.i == IS_SET || tested_paramters.v == IS_SET ||
         strcmp(tested_paramters.phrase, "search_me") == IS_NOT_SET);
  assert(tested_paramters.b == IS_NOT_SET || tested_paramters.c == IS_NOT_SET || tested_paramters.n == IS_NOT_SET ||
         tested_paramters.x == IS_NOT_SET || tested_paramters.E == IS_NOT_SET || tested_paramters.fp == stdin);
}

void test_allocate_dynamic_memory()
{
  char zero_bytes[NUMBER_OF_ARGS] = {ZERO_PARAM_INIT};
  char *tested_allocated_variable = (char *)allocate_dynamic_memory(NUMBER_OF_ARGS, sizeof(char));
  assert(memcmp(tested_allocated_variable, zero_bytes, NUMBER_OF_ARGS) == 0);
  free(tested_allocated_variable);
}

void test_tolower_string()
{
  char *tested_string;
  tested_string = "aAbBcC";
  assert(strcmp(tolower_string(tested_string), "aabbcc") == 0);
  tested_string = "AAAAAAA";
  assert(strcmp(tolower_string(tested_string), "aaaaaaa") == 0);
  tested_string = "should return the same";
  assert(strcmp(tolower_string(tested_string), tested_string) == 0);
  tested_string = "";
  assert(strcmp(tolower_string(tested_string), tested_string) == 0);
}
