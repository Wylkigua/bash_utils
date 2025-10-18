FLAGS=-Werror -Wall -Wextra -std=c11

build: s21_cat s21_test

s21_cat: main.c cat.c
	gcc $(FLAGS) cat.c main.c -o s21_cat

s21_test: s21_cat
	chmod a+x test.sh
	bash test.sh

.PHONY: clean s21_cat s21_test

clean:
	rm -rf s21_cat
