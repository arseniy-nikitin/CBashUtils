#ifndef S21_CAT
#define S21_CAT

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>

#define DEL_CODE 127
#define ASCII_SHIFT 64

typedef struct flags_state {
  int n, b, s, t, e, v, err;
} flags_state;

void parse_flags(flags_state* flag_state, int flag);
void cook_cat(int argc, char** argv, int optind, flags_state flag_state);
int process_s_flag(int s_state, int ch, int* squeeze);
void process_nb_flags(int n_state, int b_state, int ch, int* str_num);
void process_e_flag(int e_state, int ch);
int process_t_flag(int t_state, int ch);
int process_v_flag(int v_state, int ch);
void raw_cat(int argc, char** argv, int optind);

#endif