#include <assert.h>
#include "my_grep.h"

int main(){
    test_get_parameters_from_argv();
    test_tolower_string();
    test_allocate_dynamic_memory();
}

void test_get_parameters_from_argv(){
    program_arguments tested_paramters={0};
    char argv[][5] = {"search_me", "-A", "-i", "-v", ""};
    int argc = 5;
    get_parameters_from_argv(&tested_paramters, argc, argv);
    assert(tested_paramters.A!=1 || tested_paramters.i!=1 || tested_paramters.v !=1 || 
            strcmp(tested_paramters.phrase,"search_me")!=0);
    assert(tested_paramters.b!=0 || tested_paramters.c!=0 || tested_paramters.n!=0
            || tested_paramters.x!=0  || tested_paramters.E!=0 || tested_paramters.fp!=stdin);

}

void test_allocate_dynamic_memory(){
    char zero_bytes[4] = {0};
    char* tested_allocated_variable =(char*) allocate_dynamic_memory(4, sizeof(char));
    assert( sizeof(tested_allocated_variable)!=4 );
    assert( memcmp(tested_allocated_variable, zero_bytes  ,sizeof(tested_allocated_variable) )!=0 );
    free(tested_allocated_variable);
}

void test_tolower_string(){
    char tested_string[100] = {0};
    tested_string = "aAbBcC";
    assert(  strcmp(test_tolower_string(tested_string), "aabbcc") !=0 );
    tested_string = "AAAAAAA";
    assert(  strcmp(test_tolower_string(tested_string), "aaaaaaa") !=0 );
    tested_string = "should return the same";
    assert(  strcmp(test_tolower_string(tested_string), tested_string) !=0 );
    tested_string="";
    assert(  strcmp(test_tolower_string(tested_string), tested_string) !=0 );
}


