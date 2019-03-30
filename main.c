#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct arguments{
    /* perhaps an int that will represent the arguments: 1 for when only i is on, 2 when only v is on...13 for when i and v is on and so forth. it causes more switch cases but less nested cases..ie. if case==i{ if case==v do: } */
};

char* tolower_string(char* string);
void read_line(FILE* input_stream, char** line);
void is_match_in_line(char* haystack, char* needle, struct arguments args);





int main(int argc, char **argv){
    struct arguments args;
    char buffer[32];
    ssize_t  bufsize = 32;
    ssize_t  characters;
    int chars_read;
    char *phrase;
    char *line;
    FILE *fp;

    phrase = argv[1];
    if(argc == 2){ /* maybe it can be stdin with more arguments so the test if num of args is 2 isnt correct? */
        fp = stdin;
    }
    else if(argc == 3){ /* can be unlimited num of args so this needs to change probably */
        if( (fp = fopen(argv[2], "r")) == NULL){
            printf("Error while opening file. Exiting...\n");
            exit(1);
        }
    }
    else{
        printf("Error: wrong number of arguments. Exiting...\n");
        exit(1);
    }

    /**************************************** testing********************************************/
    read_line(fp, &line);
    is_match_in_line(line, phrase, args);

    /********************************************************************************************/

}



void read_line(FILE* input_stream, char** line){
    size_t len = 0;
    ssize_t bytes_read;
    bytes_read = getline(line, &len, input_stream);
    if (bytes_read == -1 ){
        printf("End of File or error reading a line!\n");
        /****** do something (check if errno is set and quit? ***********/
    }
}

void is_match_in_line(char* haystack, char* needle, struct arguments args){
    char* found;
    //switch (args){
     //   case i:
     //   {
            char* lower_case_haystack = tolower_string(haystack);
            char* lower_case_needle = tolower_string(needle);
            found = strstr(lower_case_haystack, lower_case_needle);
            free(lower_case_haystack); /* in future cases, if the lower case needs to be printed, make sure it is not freed here or printed before this line!! */
            free(lower_case_needle); /* same comment as the line above */
            if (found!=NULL){ /* if equals null, means not found, else it points to the occurrence .. */
                printf("%s\n", haystack); /* print the original line, not the lowered case one */
               // break;
            }
        //}
   // }
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