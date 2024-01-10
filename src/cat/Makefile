CC=gcc
CFLAGS=-pedantic -std=c11 -Wall -Werror -Wextra

s21_cat:
	$(CC) $(CFLAGS) s21_cat.c -o s21_cat

clean:
	rm -rf *.o s21_cat

rebuild:
	make clean
	make s21_cat

style:
	clang-format -n *.c *.h

test:
	sh test_func_cat.sh
	sh test_leak_cat.sh