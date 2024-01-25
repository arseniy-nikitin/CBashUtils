CC=gcc
CFLAGS=-pedantic -std=c11 -Wall -Werror -Wextra

s21_cat:
	$(CC) $(CFLAGS) s21_cat.c -o s21_cat

clean:
	rm -rf *.o s21_cat

rebuild:
	make clean
	make s21_cat

style-replace:
	clang-format --style=Google *.c *.h -i

style-check:
	clang-format --style=Google *.c *.h -n

tests:
	cd tests && sh test_func_cat.sh

leaks:
	cd tests && sh test_leak_cat.sh

.PHONY: s21_cat tests