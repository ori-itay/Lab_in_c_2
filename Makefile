.PHONY: all clean test
CC=gcc
OBJS= my_grep.c
EXEC = all
COMP_FLAG =-Wall -Wextra -Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC)  $(COMP_FLAG) $(OBJS) -o my_grep 
clean:
	rm -rf *.o my_grep
test:
	/specific/a/home/cc/students/csguests/nimrodav/grep_tests/beta1.sh /specific/a/home/cc/students/cs/itaylevi/c_lab/ex2/beta1/my_grep