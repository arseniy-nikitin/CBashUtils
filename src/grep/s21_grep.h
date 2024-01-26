#ifndef S21_GREP
#define S21_GREP

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct flags_state {
  int e, i, v, c, l, n, h, s, f, o, multiple, err;
} flags_state;

typedef struct cycle_flags {
  int c_matches, l_match, line_match, v_matches;
} cycle_flags;

void parse_flags(flags_state* flags, char** dynamic_array, int option);
void grep(flags_state flags, char** arguments, FILE* fp, char* file_name);
int file_state(FILE* fp, char* file_name);
char* my_strcasestr(const char* __big, const char* __little);
void print_prefixes(flags_state flags, char* file_name, int* prev_line_num,
                    int line_num);
void print_o_matches(char* line, regex_t* regex, regmatch_t* match);
void get_line_match(flags_state flags, cycle_flags* cycle_state, char* line);
void print_cl_matches(flags_state flags, cycle_flags cycle_state,
                      char* file_name);

#endif