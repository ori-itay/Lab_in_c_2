#include <stdio.h>
#include <string.h>



struct arguments{

};



int main(int argc, char **argv){

   /* char buffer[32];
    ssize_t  bufsize = 32;
    ssize_t  characters;
    int chars_read;
    char *phrase;
    FILE *fp;

    phrase = argv[1];
    if(argc == 2){
        fp = STDIN_FILENO;
    }
    else if(argc == 3){
        if( (fp = fopen(argv[2], "r")) == NULL){
            printf("Error while opening file. Exiting...\n");
            exit(1);
        }
    }
    else{
        printf("Error: wrong number of arguments. Exiting...\n");
        exit(1);
    }


    while( (chrars_read = getline(buffer, &bufsize, FILE *stream) ){

    }*/


}



void read_line(FILE* input_stream, char* line){
    size_t len = 0;
    ssize_t bytes_read;
    bytes_read = getline(&line, &len, input_stream);
    if (bytes_read == -1 ){
        printf("End of File or error reading a line!\n");
        /****** do something (check if errno is set and quit? ***********/
    }
}

void is_match_in_line(char* haystack, char* needle, struct arguments args){
    switch (args){
        case i:
        {
            char* lower_case_line = tolower_line(haystack);
            char * lower_case_needle = tolower_line(needle);
        }
    }
}