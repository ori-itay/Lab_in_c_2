#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

struct arguments{
    /* perhaps an int that will represent the arguments: 1 for when only i is on, 2 when only v is on...13 for when i and v is on and so forth. it causes more switch cases but less nested cases..ie. if case==i{ if case==v do: } */
    int A, NUM, b, c, i, n, v, x;
};
struct LINE{
    char *line_string_ptr;
    int curr_line_num, char_offset, A_line_offset;
};

char* tolower_string(char* string);
int read_line(FILE* input_stream, char**);
void is_match_in_line(struct LINE *line_args, char* needle, struct arguments *params, int *line_matched_count);
void get_arguments_from_argv(struct arguments *params, FILE **fp, int *num_of_params,char** phrase, int argc, char **argv);
int is_match(char* haystack, char* needle, int x);
int report_line_match(char* haystack, char* needle, struct arguments *params);


int main(int argc, char **argv){
    struct arguments params = {0,0,0,0,0,0,0,0};
    struct LINE line_args = {0,1,0,-1};
    int num_of_params, line_matched_count = 0, curr_line_num = 1, bytes_read;
    char *phrase, *line;
    FILE *fp;

    get_arguments_from_argv(&params, &fp, &num_of_params, &phrase, argc, argv);
    while( (bytes_read = read_line(fp, &line)) > 0){
        line_args.line_string_ptr = line;
        is_match_in_line(&line_args, phrase, &params, &line_matched_count);
        line_args.curr_line_num ++;
        line_args.char_offset+= bytes_read;
    }
    if(params.c)
        printf("%d",line_matched_count);

    return 0;

    /**************************************** testing********************************************/
    //read_line(fp, &line);
    //is_match_in_line(line, phrase, &params, &line_matched_count, curr_line_num, char_offset); // also check this test for file as input xD

    /********************************************************************************************/
    //return 0;
}



int read_line(FILE* input_stream, char** line){
    size_t len = 0;
    ssize_t bytes_read;

    bytes_read = getline(line, &len, input_stream);
    if (bytes_read == -1 ){
        if(errno){
            printf("End of File or error reading a line!\n");
        }
    }
    return bytes_read;
}

void is_match_in_line(struct LINE *line_args, char* needle, struct arguments *params, int *line_matched_count){

    int found;
    char *haystack = line_args->line_string_ptr;

    if( (found = report_line_match(haystack, needle, params)) ){

        (*line_matched_count)++;
        if (params->c)
            return;

        if(params->n)
            printf("%d:", line_args->curr_line_num);
        if(params->b)
            printf("%d:", line_args->char_offset);
        printf("%s", haystack);

        if(params->A)
            line_args->A_line_offset = params->NUM + 1;

    }
    else if(params->A && line_args->A_line_offset > 0){
        printf("%d-", line_args->curr_line_num);
        if(params->b)
            printf("%d-", line_args->char_offset);
        printf("%s", haystack);

    }
    else if(line_args->A_line_offset == 0)
        printf("--\n");
    (line_args->A_line_offset)--;

    return;
}

int report_line_match(char* haystack, char* needle, struct arguments *params) {
    int match;

    if (params->i) {
        /* this case at the moment fits for when only -i is on (make changes according to the struct etc, discuss with ori */
        char* lower_case_haystack = tolower_string(haystack);
        char* lower_case_needle = tolower_string(needle);
        match = is_match(lower_case_haystack, lower_case_needle, params->x);
        free(lower_case_haystack); /* in future cases, if the lower case needs to be printed, make sure it is not freed here or print it before this line!! */
        free(lower_case_needle); /* same comment as the line above */
    }
    else{
        match = is_match(haystack, needle, params->x);
    }

    match^= params->v;

    return match;
}

int is_match(char* haystack, char* needle, int x){
    int line_length = strlen(haystack) - 1;//without '\n'

    if (x && line_length) {
        return (strncmp(haystack, needle, line_length) == 0);
    }
    else {
        return (strstr(haystack, needle)!= NULL);
    }
}


char* tolower_string(char* string){
    int index;
    int length = strlen(string);
    char* lowered_string = (char*)calloc(length + 1, sizeof(char));
    strncpy(lowered_string, string, length);
    for (index = 0 ; index < length ; index++){
        lowered_string[index] = tolower(string[index]);
    }
    return lowered_string;
}

void get_arguments_from_argv(struct arguments *params, FILE **fp, int *num_of_params,char** phrase, int argc, char **argv){
    int ind;

    if(argc < 2){
        printf("Error: wrong number of arguments. Exiting...\n");
        exit(1);
    }
    else if (argc == 2 || argv[argc-1][0] == '-'){
        //no file entered as argument
        *fp = STDIN_FILENO;
        *num_of_params = argc - 2;
    }
    else{
        if( (*fp = fopen(argv[argc-1], "r")) == NULL){  /* problem with relative path!!! */
            printf("Error while opening file. Exiting...\n");
            exit(1);
        }
        *num_of_params = argc - 3;
    }
    *phrase = argv[1];

    for(ind = 2; ind<(*num_of_params)+2; ind++){
        if(strcmp(argv[ind], "-A") == 0){
            params->A = 1;
            params->NUM = (int) strtoul(argv[ind+1], NULL, 10);
            ind++; // pass over next argument
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
    }
    return;
}
