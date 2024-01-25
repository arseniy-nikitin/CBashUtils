CC=gcc
CFLAGS=-pedantic -std=c11 -Wall -Werror -Wextra

all: s21_grep

s21_grep:
	$(CC) $(CFLAGS) s21_grep.c ../common/dynamic_db.c -o s21_grep

clean:
	rm -rf *.o s21_grep

rebuild:
	make clean
	make s21_grep

style-replace:
	clang-format --style=Google *.c *.h -i
	clang-format --style=Google ../common/*.c ../common/*.h -i

style-check:
	clang-format --style=Google *.c *.h -n
	clang-format --style=Google ../common/*.c ../common/*.h -n

tests:
	cd tests && sh test_func_grep.sh

leaks:
	cd tests && sh test_leak_grep.sh

.PHONY: s21_grep tests