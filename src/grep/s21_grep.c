#include "s21_grep.h"

#include "../common/dynamic_db.h"

int main(int argc, char** argv) {
  if (argc < 3) return 0;
  char** dynamic_array = NULL;
  dynamic_array = init_db(dynamic_array);
  if (dynamic_array == NULL) return 0;
  flags_state flags = {0};
  const char* short_flags = "e:ivclnhsf:o";
  const struct option long_flags[] = {{NULL, 0, NULL, 0}};
  int actual_flag = 0;
  while (actual_flag != -1) {
    actual_flag = getopt_long(argc, argv, short_flags, long_flags, NULL);
    parse_flags(&flags, dynamic_array, actual_flag);
    if (dynamic_array == NULL) return 0;
  }
  if (!flags.err) {
    if (!flags.e && !flags.f) {
      dynamic_array = add_single_db(dynamic_array, argv[optind++]);
      if (dynamic_array == NULL) return 0;
    }
    FILE* fp = NULL;
    flags.multiple = argv[optind + 1] ? 1 : 0;
    if (flags.l && !flags.c) flags.multiple = 0;
    while (argv[optind]) {
      if ((fp = fopen(argv[optind], "r"))) {
        if (file_state(fp, argv[optind])) {
          grep(flags, dynamic_array, fp, argv[optind]);
        }
      } else {
        fprintf(stderr, "s21_grep: %s: No such file or directory\n",
                argv[optind]);
      }
      if (fp != NULL) {
        fclose(fp);
      }
      optind++;
    }
  } else {
    fprintf(stderr,
            "usage: grep [-eivclnhsfo]\n\
              [-e pattern] [-f file]\n\
              [pattern] [file ...]\n");
  }
  if (dynamic_array != NULL) {
    free_db(dynamic_array);
  }
  return 0;
}

void parse_flags(flags_state* flags, char** dynamic_array, int option) {
  if (option == 'e') {
    flags->e = 1;
    dynamic_array = add_single_db(dynamic_array, optarg);
  } else if (option == 'i') {
    flags->i = 1;
  } else if (option == 'v') {
    flags->v = 1;
    flags->o = 0;
  } else if (option == 'c') {
    flags->c = 1;
    flags->o = 0;
    flags->n = 0;
  } else if (option == 'l') {
    flags->l = 1;
    flags->o = 0;
    flags->n = 0;
  } else if (option == 'n') {
    if (!flags->c && !flags->l) flags->n = 1;
  } else if (option == 'h') {
    flags->h = 1;
  } else if (option == 's') {
    flags->s = 1;
  } else if (option == 'f') {
    flags->f = 1;
    dynamic_array = add_list_db(dynamic_array, optarg);
  } else if (option == 'o') {
    if (!flags->v && !flags->c && !flags->l) flags->o = 1;
  } else if (option == '?') {
    flags->err = 1;
  }
}

void grep(flags_state flags, char** arguments, FILE* fp, char* file_name) {
  cycle_flags cycle_state = {0};
  regex_t regex;
  regmatch_t match;
  char* line = NULL;
  int line_num = 1;
  int prev_line_num = 0;
  ssize_t ch = 0;
  size_t n = 0;
  while ((ch = getline(&line, &n, fp)) != -1) {
    while (ch > 0 && (line[ch - 1] == '\r' || line[ch - 1] == '\n')) {
      line[--ch] = 0;
    }
    char* line_ptr = line;
    cycle_state.line_match = 0;
    cycle_state.v_matches = 0;
    for (int i = 0; arguments[i]; i++) {
      int ret = regcomp(&regex, arguments[i], flags.i ? REG_ICASE : 0);
      if (!ret) {
        ret = regexec(&regex, line_ptr, 1, &match, 0);
      } else {
        char* pre_ret = flags.i ? my_strcasestr(line, arguments[i])
                                : strstr(line, arguments[i]);
        ret = pre_ret ? 0 : 1;
      }
      if (!cycle_state.line_match) {
        if (!flags.v && !ret) {
          print_prefixes(flags, file_name, &prev_line_num, line_num);
          if (flags.o) {
            print_o_matches(line, &regex, &match);
          } else {
            get_line_match(flags, &cycle_state, line);
          }
        } else if (flags.v && ret) {
          cycle_state.v_matches++;
          if (cycle_state.v_matches == arrind) {
            print_prefixes(flags, file_name, &prev_line_num, line_num);
            get_line_match(flags, &cycle_state, line);
          }
        }
      }
      regfree(&regex);
    }
    line_num++;
  }
  print_cl_matches(flags, cycle_state, file_name);
  if (line) free(line);
}

int file_state(FILE* fp, char* file_name) {
  int is_readble = 1;
  struct stat file_stat;
  if (fstat(fileno(fp), &file_stat) == -1) {
    fprintf(stderr, "s21_grep: %s: Unable to get file state\n", file_name);
    is_readble = 0;
  } else {
    if (S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR)) {
      printf("Binary file %s matches\n", file_name);
      is_readble = 1;
    } else if (S_ISDIR(file_stat.st_mode)) {
      printf("s21_grep: %s: Is a directory\n", file_name);
      is_readble = 0;
    }
  }
  return is_readble;
}

char* my_strcasestr(const char* __big, const char* __little) {
  char* result = NULL;
  if (__big && __little && *__little) {
    while (*__big && !result) {
      const char* __bigptr = __big;
      const char* __littleptr = __little;
      while (*__bigptr && *__littleptr &&
             (tolower(*__bigptr) == tolower(*__littleptr))) {
        __bigptr++;
        __littleptr++;
      }
      if (!*__littleptr)
        result = (char*)__big;
      else
        __big++;
    }
  }
  return result;
}

void print_prefixes(flags_state flags, char* file_name, int* prev_line_num,
                    int line_num) {
  if (*prev_line_num != line_num) {
    if (flags.multiple && !flags.h && !flags.c) printf("%s:", file_name);
    if (flags.n) printf("%d:", line_num);
    *prev_line_num = line_num;
  }
}

void print_o_matches(char* line, regex_t* regex, regmatch_t* match) {
  char* line_ptr = line;
  int ret = regexec(regex, line_ptr, 1, match, 0);
  while (!ret) {
    int m_start = match->rm_so + (line_ptr - line);
    int m_end = match->rm_eo + (line_ptr - line);
    printf("%.*s\n", m_end - m_start, &line[m_start]);
    line_ptr += match->rm_eo;
    ret = regexec(regex, line_ptr, 1, match, 0);
  }
}

void get_line_match(flags_state flags, cycle_flags* cycle_state, char* line) {
  if (flags.c) {
    if (!cycle_state->l_match) cycle_state->c_matches++;
  }
  if (flags.l) cycle_state->l_match = 1;
  if (!flags.o && !flags.c && !flags.l) printf("%s\n", line);
  cycle_state->line_match = 1;
}

void print_cl_matches(flags_state flags, cycle_flags cycle_state,
                      char* file_name) {
  if (flags.c) {
    if (flags.multiple && !flags.h) printf("%s:", file_name);
    printf("%d\n", cycle_state.c_matches);
  }
  if (flags.l && cycle_state.l_match) printf("%s\n", file_name);
}