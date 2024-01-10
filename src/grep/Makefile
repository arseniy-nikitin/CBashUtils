CC=gcc
CFLAGS=-pedantic -std=c11 -Wall -Werror -Wextra

s21_grep:
	$(CC) $(CFLAGS) s21_grep.c ../common/dynamic_db.c -o s21_grep

clean:
	rm -rf *.o s21_grep

rebuild:
	make clean
	make s21_grep

style:
	clang-format -n *.c *.h

test:
	sh test_func_grep.sh
	sh test_leak_grep.sh