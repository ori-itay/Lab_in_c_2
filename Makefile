.PHONY: all clean test
all: my_grep.c
	gcc -Wall -Wextra -Werror -pedantic-errors my_grep.c -o my_grep
clean:
	rm -rf *.o my_grep
test:
	/specific/a/home/cc/students/csguests/nimrodav/grep_tests/beta1.sh /specific/a/home/cc/students/cs/itaylevi/c_lab/ex2/beta1/my_grep