#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define RANGE_START_OFFSET 1
#define RANGE_END_OFFSET 3

struct arguments{
    int A, NUM, b, c, i, n, v, x, E;
    FILE *fp;
};
struct LINE{
    char *line_string_ptr;
    int curr_line_num, char_offset, A_line_offset;
};
typedef enum component_type{
REGULAR_CHAR,
ESCAPE_BACKSLASH,
DOT,
SQUARED_BRACKETS,
CIRCULAR_BRACKETS
} component_type;

typedef struct PHRASE_COMPONENT{
    component_type type;
    int range_start, range_end, start_index_in_phrase, end_index_in_phrase;
    char checked_char;
} phrase_component;


void get_params_from_argv(struct arguments *params, char** phrase, int argc, char **argv);
char* tolower_string(char* string);
void report_line_match(struct LINE *line_args, char* needle, struct arguments *params, int *line_matched_count,
        phrase_component *components_list, int components_count);
int is_match_in_line(char* haystack, char* needle, struct arguments *params, phrase_component *components_list, int components_count);
int is_match_at_place(char* haystack, char* needle, int component_index, phrase_component* component_list, int component_count, struct arguments *params);
int parse_line(char *orig_string, phrase_component** component_list);


int main(int argc, char **argv){
    struct arguments params = {0,0,0,0,0,0,0,0,0, NULL};
    struct LINE line_args = {0,1,0,-1};
    int line_matched_count = 0, bytes_read, components_count = 0;
    char *phrase, *line = NULL;
    size_t line_len = 0;
    phrase_component* components_list;

    get_params_from_argv(&params, &phrase, argc, argv);
    if(params.i) {
        phrase = tolower_string(phrase);}
    if( (components_list = (phrase_component*)calloc(strlen(phrase), sizeof(phrase_component)) ) == NULL){
        printf("Error while allocating memory. exiting...\n");
        exit(1);}

    components_count = parse_line(phrase, &components_list);
    while( (bytes_read = getline(&line, &line_len, params.fp)) > 0){
        if (bytes_read == -1 ){
            if(errno){
                printf("End of File or error reading a line!\n");
                free(line);
                exit(1);}
        }
        line_args.line_string_ptr = line;
        report_line_match(&line_args, phrase, &params, &line_matched_count, components_list, components_count);
        line_args.curr_line_num ++;
        line_args.char_offset+= bytes_read;
    }
    if(params.c)
        printf("%d\n",line_matched_count);
    if(params.i)
        free(phrase);
    free(components_list);
    free(line);
    fclose(params.fp);
    return 0;
}


void report_line_match(struct LINE *line_args, char* needle, struct arguments *params, int *line_matched_count,
                       phrase_component *components_list, int components_count){

    int match = 0;
    char *haystack = line_args->line_string_ptr;

    if( (match = is_match_in_line(haystack, needle, params, components_list, components_count)) ){

        if(line_args->A_line_offset == 0)
            printf("--\n");

        (*line_matched_count)++;
        if (params->c)
            return;

        if(params->n)
            printf("%d:", line_args->curr_line_num);
        if(params->b)
            printf("%d:", line_args->char_offset);
        printf("%s", haystack);

        if(params->A)
            line_args->A_line_offset = params->NUM;

    }
    else if(params->A && line_args->A_line_offset > 0){
        if(params->n)
            printf("%d-", line_args->curr_line_num);
        if(params->b)
            printf("%d-", line_args->char_offset);
        printf("%s", haystack);
        if(line_args->A_line_offset > 0)
            (line_args->A_line_offset)--;
    }
    return;
}

int is_match_in_line(char* haystack, char* needle, struct arguments *params, phrase_component *components_list, int components_count) {
    int match = 0, needle_len = strlen(needle), haystack_len = (strlen(haystack)), component_index = 0, haystack_index = 0;

    if (params->i) {
        haystack = tolower_string(haystack);
    }
    if(params->x) {
        if(needle_len == haystack_len-1)
            match = is_match_at_place(haystack, needle, component_index, components_list, components_count, params);
    }
    else{
        for(; haystack_index<haystack_len; haystack_index++){
            if( (match = is_match_at_place(haystack+haystack_index, needle, component_index, components_list, components_count, params)) )
                break;
        }
    }
    match^= params->v;

    if(params->i)
        free(haystack);
    return match;
}

int parse_line(char *orig_string, phrase_component** components_list){

    int str_len = strlen(orig_string), string_index, component_index = 0;

    for(string_index = 0; string_index<str_len; string_index++){
        if(orig_string[string_index] == '.') {
            (*components_list)[component_index].type = DOT;
        }
        else if(orig_string[string_index] == '\\'){
            string_index++;
            (*components_list)[component_index].type = ESCAPE_BACKSLASH;
            (*components_list)[component_index].checked_char = orig_string[string_index];
        }
        else if(orig_string[string_index] == '['){
            (*components_list)[component_index].type = SQUARED_BRACKETS;
            (*components_list)[component_index].range_start = orig_string[string_index+RANGE_START_OFFSET];
            string_index+= RANGE_END_OFFSET;
            (*components_list)[component_index].range_end = orig_string[string_index];
            string_index++;
        }
        else if(orig_string[string_index] == '(') {
            (*components_list)[component_index].type = CIRCULAR_BRACKETS;
            (*components_list)[component_index].start_index_in_phrase = string_index;
            while (orig_string[++string_index] != ')') {}
            (*components_list)[component_index].end_index_in_phrase = string_index;
        }
        else{
            (*components_list)[component_index].type = REGULAR_CHAR;
            (*components_list)[component_index].checked_char = orig_string[string_index];
        }
        component_index++;
    }
    return component_index;
}


int is_match_at_place(char* haystack, char* needle, int component_index, phrase_component* component_list,
        int component_count, struct arguments *params){

    int match = 0, current_string_index, compare_length = 0, component_end_index;

    if(component_index >= component_count) {
        match = TRUE;
    }
    else if( ( (component_list[component_index].type == REGULAR_CHAR || component_list[component_index].type == ESCAPE_BACKSLASH) &&
            component_list[component_index].checked_char == haystack[0] ) ||
            component_list[component_index].type == DOT ||
            (component_list[component_index].type == SQUARED_BRACKETS && component_list[component_index].range_start <= haystack[0]
             && component_list[component_index].range_end >= haystack[0]) ){
        match = is_match_at_place(haystack+1, needle, component_index+1, component_list, component_count, params);
    }
    else if( component_list[component_index].type == CIRCULAR_BRACKETS){
        current_string_index = component_list[component_index].start_index_in_phrase;
        component_end_index = component_list[component_index].end_index_in_phrase;
        while(current_string_index < component_end_index) {
            while (needle[++current_string_index] != '|' && current_string_index < component_end_index) {
                compare_length++;}
            if (strncmp(needle + current_string_index-compare_length, haystack, compare_length) == 0)
                match = is_match_at_place(haystack + compare_length, needle, component_index + 1, component_list, component_count, params);
            compare_length = 0;
            if(match)
                break;
        }
    }

    else{
        match = FALSE;
    }
    return match;
}


char* tolower_string(char* string){
    int index;
    int length = strlen(string);
    char* lowered_string = (char*)calloc(length + 1, sizeof(char));
    if(lowered_string == NULL){
        printf("Error while allocating memory. exiting...\n");
        exit(1);
    }
    strncpy(lowered_string, string, length);
    for (index = 0 ; index < length ; index++){
        lowered_string[index] = tolower(string[index]);
    }
    return lowered_string;
}

void get_params_from_argv(struct arguments *params, char** phrase, int argc, char **argv){
    int ind;
    *phrase = NULL;

    if(argc < 2){
        printf("Error: wrong number of arguments. Exiting...\n");
        exit(1);
    }

    for(ind = 1; ind <argc; ind++){
        if(argv[ind][0] != '-' && *phrase == NULL)
            *phrase = argv[ind];
        else if(ind == argc-1 && argv[ind][0] != '-' && *phrase != NULL){
            if( (params->fp = fopen(argv[argc-1], "r")) == NULL){
                printf("Error while opening file. Exiting...\n");
                exit(1);
            }
        }
        else if(strcmp(argv[ind], "-A") == 0){
            params->A = 1;
            params->NUM = (int) strtoul(argv[ind+1], NULL, 10);
            ind++;
        }
        else if(strcmp(argv[ind], "-b") == 0)
            params->b = 1;
        else if(strcmp(argv[ind], "-c") == 0)
            params->c = 1;
        else if(strcmp(argv[ind], "-i") == 0)
            params->i = 1;
        else if(strcmp(argv[ind], "-n") == 0)
            params->n = 1;
        else if(strcmp(argv[ind], "-v") == 0)
            params->v = 1;
        else if(strcmp(argv[ind], "-x") == 0)
            params->x = 1;
        else if(strcmp(argv[ind], "-E") == 0){
            params->E = 1;
        }
    }
    if(params->fp == NULL)
        params->fp = stdin;
    return;
}