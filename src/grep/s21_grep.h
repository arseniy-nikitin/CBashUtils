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

typedef struct program_flags {
  int e_flag, i_flag, v_flag, c_flag, l_flag, n_flag, h_flag, s_flag, f_flag,
      o_flag, err_flag, multiple_files;
} program_flags;

typedef struct cycle_flags {
  int c_matches, l_match, line_match, v_matches;
} cycle_flags;

void parse_flags(program_flags* flag_state, char** dynamic_array, int option);
void grep(program_flags flag_state, char** arguments, FILE* fp,
          char* file_name);
int file_state(FILE* fp, char* file_name);
char* my_strcasestr(const char* __big, const char* __little);
void print_prefixes(program_flags flag_state, char* file_name,
                    int* prev_line_num, int line_num);
void print_o_matches(char* line, regex_t* regex, regmatch_t* match);
void get_line_match(program_flags flag_state, cycle_flags* cycle_state,
                    char* line);
void print_cl_matches(program_flags flag_state, cycle_flags cycle_state,
                      char* file_name);

#endif