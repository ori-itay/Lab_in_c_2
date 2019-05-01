.PHONY: all clean test
CC=gcc
OBJS=main.o input_parser.o pattern_search.o 
EXEC = my_grep
COMP_FLAG =-Wall -Wextra -Werror -pedantic-errors

$(EXEC): $(OBJS)
	@$(CC)  $(COMP_FLAG) $(OBJS) -o my_grep

main.o: main.c input_parser.h 
	@$(CC) $(COMP_FLAG) -c $*.c
input_parser.o: input_parser.c input_parser.h 
	@$(CC) $(COMP_FLAG) -c $*.c
pattern_search.o: pattern_search.c input_parser.h 
	@$(CC) $(COMP_FLAG) -c $*.c

input_parser_test: input_parser_test.c input_parser.h 
	@$(CC) $(COMP_FLAG) input_parser_test.c input_parser.c -o input_parser_test
pattern_search_test: pattern_search_test.c input_parser.h 
	@$(CC) $(COMP_FLAG) pattern_search_test.c input_parser.c pattern_search.c -o pattern_search_test

all: $(OBJS)
	@$(CC)  $(COMP_FLAG) $(OBJS) -o my_grep
clean:
	@rm -rf *.o my_grep input_parser_test pattern_search_test
test:
	@/specific/a/home/cc/students/csguests/nimrodav/grep_tests/run_all.sh