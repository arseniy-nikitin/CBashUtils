#include "s21_cat.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    flags_state flags = {0};
    int actual_flag = 0;
    const char* short_flags = "+nbstTeEv";
    const struct option long_flags[] = {
        {"number", no_argument, NULL, 'n'},
        {"number-nonblank", no_argument, NULL, 'b'},
        {"squeeze-blank", no_argument, NULL, 's'},
        {NULL, 0, NULL, 0}};
    while (actual_flag != -1) {
      actual_flag = getopt_long(argc, argv, short_flags, long_flags, NULL);
      parse_flags(&flags, actual_flag);
    }
    if (!flags.err && optind != argc) {
      if (flags.n || flags.b || flags.s || flags.t || flags.e || flags.v) {
        cook_cat(argc, argv, optind, flags);
      } else {
        raw_cat(argc, argv, optind);
      }
    } else {
      fprintf(stderr, "usage: s21_cat [-nbstTeEv] [file ...]\n");
    }
  } else {
    fprintf(stderr, "s21_cat: too few arguments");
  }
  return 0;
}

void parse_flags(flags_state* flags, int actual_flag) {
  if (actual_flag == 'n')
    flags->n = 1;
  else if (actual_flag == 'b')
    flags->b = flags->n = 1;
  else if (actual_flag == 's')
    flags->s = 1;
  else if (actual_flag == 't')
    flags->t = flags->v = 1;
  else if (actual_flag == 'T')
    flags->t = 1;
  else if (actual_flag == 'e')
    flags->e = flags->v = 1;
  else if (actual_flag == 'E')
    flags->e = 1;
  else if (actual_flag == 'v')
    flags->v = 1;
  else if (actual_flag == '?')
    flags->err = 1;
}

void cook_cat(int argc, char** argv, int optind, flags_state flags) {
  FILE* fp = NULL;
  while (optind < argc) {
    fp = fopen(argv[optind], "rb+");
    if (fp == NULL) {
      fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[optind]);
      optind++;
      continue;
    }
    int prev = 0, ch = 0, squeeze = 0, str_num = 1;
    for (prev = '\n'; (ch = fgetc(fp)) != EOF; prev = ch) {
      if (prev == '\n') {
        if (process_s_flag(flags.s, ch, &squeeze)) continue;
        process_nb_flags(flags.n, flags.b, ch, &str_num);
      }
      if (process_t_flag(flags.t, ch)) continue;
      if (process_v_flag(flags.v, ch)) continue;
      process_e_flag(flags.e, ch);
      putchar(ch);
    }
    if (fp != NULL) {
      fclose(fp);
    }
    optind++;
  }
  fp = NULL;
}

int process_s_flag(int s_state, int ch, int* squeeze) {
  int skip_ch = 0;
  if (s_state) {
    if (ch == '\n') {
      if (*squeeze == 1) skip_ch = 1;
      *squeeze = 1;
    } else
      *squeeze = 0;
  }
  return skip_ch;
}

void process_nb_flags(int n_state, int b_state, int ch, int* str_num) {
  if (n_state && (!b_state || ch != '\n')) printf("%6d\t", (*str_num)++);
}

void process_e_flag(int e_state, int ch) {
  if (ch == '\n' && e_state) putchar('$');
}

int process_t_flag(int t_state, int ch) {
  int skip_ch = 0;
  if (ch == '\t' && t_state) {
    putchar('^');
    putchar('I');
    skip_ch = 1;
  }
  return skip_ch;
}

int process_v_flag(int v_state, int ch) {
  int skip_ch = 0;
  if (v_state) {
    if (iscntrl(ch) && ch != '\t' && ch != '\n') {
      putchar('^');
      putchar(ch == DEL_CODE ? '?' : ch + ASCII_SHIFT);
      skip_ch = 1;
    }
  }
  return skip_ch;
}

void raw_cat(int argc, char** argv, int optind) {
  FILE* fp = NULL;
  while (optind < argc) {
    fp = fopen(argv[optind], "rb+");
    if (fp) {
      int ch;
      while ((ch = fgetc(fp)) != EOF) putchar(ch);
    } else {
      fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[optind]);
    }
    if (fp != NULL) {
      fclose(fp);
    }
    optind++;
  }
  fp = NULL;
}