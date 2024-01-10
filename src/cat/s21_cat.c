#include "s21_cat.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    flags flag_state = {0};
    int flag = 0;
    const char* short_flags = "+nbstTeEv";
    const struct option long_flags[] = {
        {"number", no_argument, NULL, 'n'},
        {"number-nonblank", no_argument, NULL, 'b'},
        {"squeeze-blank", no_argument, NULL, 's'},
        {NULL, 0, NULL, 0}};
    while ((flag = getopt_long(argc, argv, short_flags, long_flags, NULL)) !=
           -1)
      parse_flags(&flag_state, flag);
    if (!flag_state.errflag && optind != argc) {
      if (flag_state.nflag || flag_state.bflag || flag_state.sflag ||
          flag_state.tflag || flag_state.eflag || flag_state.vflag)
        cook_cat(argc, argv, optind, flag_state);
      else
        raw_cat(argc, argv, optind);
    } else
      fprintf(stderr, "usage: s21_cat [-nbstTeEv] [file ...]\n");
  } else
    fprintf(stderr, "s21_cat: too few arguments");
  return 0;
}

void parse_flags(flags* flag_state, int flag) {
  if (flag == 'n')
    flag_state->nflag = 1;
  else if (flag == 'b')
    flag_state->bflag = flag_state->nflag = 1;
  else if (flag == 's')
    flag_state->sflag = 1;
  else if (flag == 't')
    flag_state->tflag = flag_state->vflag = 1;
  else if (flag == 'T')
    flag_state->tflag = 1;
  else if (flag == 'e')
    flag_state->eflag = flag_state->vflag = 1;
  else if (flag == 'E')
    flag_state->eflag = 1;
  else if (flag == 'v')
    flag_state->vflag = 1;
  else if (flag == '?')
    flag_state->errflag = 1;
}

void cook_cat(int argc, char** argv, int optind, flags flag_state) {
  FILE* fp = NULL;
  while (optind < argc) {
    fp = fopen(argv[optind], "rb+");
    if (fp) {
      int prev = 0, ch = 0, squeeze = 0, str_num = 1;
      for (prev = '\n'; (ch = fgetc(fp)) != EOF; prev = ch) {
        if (prev == '\n') {
          if (flag_state.sflag) {
            if (ch == '\n') {
              if (squeeze) continue;
              squeeze = 1;
            } else
              squeeze = 0;
          }
          if (flag_state.nflag && (!flag_state.bflag || ch != '\n'))
            printf("%6d\t", str_num++);
        }
        if (ch == '\n' && flag_state.eflag)
          putchar('$');
        else if (ch == '\t' && flag_state.tflag) {
          putchar('^');
          putchar('I');
          continue;
        } else if (flag_state.vflag) {
          if (iscntrl(ch) && ch != '\t' && ch != '\n') {
            putchar('^');
            putchar(ch == 127 ? '?' : ch + 64);
            continue;
          }
        }
        putchar(ch);
      }
    } else
      fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[optind]);
    fclose(fp);
    optind++;
  }
  fp = NULL;
}

void raw_cat(int argc, char** argv, int optind) {
  FILE* fp = NULL;
  while (optind < argc) {
    fp = fopen(argv[optind], "rb+");
    if (fp) {
      int ch;
      while ((ch = fgetc(fp)) != EOF) putchar(ch);
    } else
      fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[optind]);
    fclose(fp);
    optind++;
  }
  fp = NULL;
}