#ifndef S21_CAT
#define S21_CAT

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>

typedef struct flag_list {
  int nflag, bflag, sflag, tflag, eflag, vflag, errflag;
} flags;

void parse_flags(flags* flag_state, int flag);
void cook_cat(int argc, char** argv, int optind, flags flag_state);
void raw_cat(int argc, char** argv, int optind);

#endif