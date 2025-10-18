FLAGS=-Werror -Wall -Wextra -std=c11

build: s21_grep s21_test

s21_grep: main.c grep.c
	gcc $(FLAGS) grep.c main.c -o s21_grep

s21_test: s21_grep
	chmod a+x test.sh
	bash test.sh

.PHONY: clean s21_grep s21_test

clean:
	rm -rf s21_grep
