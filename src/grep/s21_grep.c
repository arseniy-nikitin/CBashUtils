#include "s21_grep.h"

#include "../common/dynamic_db.h"

int main(int argc, char** argv) {
  if (argc > 2) {
    program_flags flag_state = {0};
    const char* short_flags = "e:ivclnhsf:o";
    const struct option long_flags[] = {{NULL, 0, NULL, 0}};
    int option = 0;
    char** dynamic_array = NULL;
    dynamic_array = init_db(dynamic_array);
    while ((option = getopt_long(argc, argv, short_flags, long_flags, NULL)) !=
           -1)
      parse_flags(&flag_state, dynamic_array, option);
    if (!flag_state.err_flag) {
      if (!flag_state.e_flag && !flag_state.f_flag)
        dynamic_array = add_single_db(dynamic_array, argv[optind++]);
      FILE* fp = NULL;
      flag_state.multiple_files = argv[optind + 1] ? 1 : 0;
      if (flag_state.l_flag && !flag_state.c_flag)
        flag_state.multiple_files = 0;
      while (argv[optind]) {
        if ((fp = fopen(argv[optind], "r"))) {
          if (file_state(fp, argv[optind]))
            grep(flag_state, dynamic_array, fp, argv[optind]);
        } else
          fprintf(stderr, "s21_grep: %s: No such file or directory\n",
                  argv[optind]);
        fclose(fp);
        optind++;
      }
    } else
      fprintf(stderr,
              "usage: grep [-eivclnhsfo]\n\
              [-e pattern] [-f file]\n\
              [pattern] [file ...]\n");
    free_db(dynamic_array);
  } else
    fprintf(stderr, "s21_grep: too few arguments");
  return 0;
}

void parse_flags(program_flags* flag_state, char** dynamic_array, int option) {
  if (option == 'e') {
    flag_state->e_flag = 1;
    dynamic_array = add_single_db(dynamic_array, optarg);
  } else if (option == 'i') {
    flag_state->i_flag = 1;
  } else if (option == 'v') {
    flag_state->v_flag = 1;
    flag_state->o_flag = 0;
  } else if (option == 'c') {
    flag_state->c_flag = 1;
    flag_state->o_flag = 0;
    flag_state->n_flag = 0;
  } else if (option == 'l') {
    flag_state->l_flag = 1;
    flag_state->o_flag = 0;
    flag_state->n_flag = 0;
  } else if (option == 'n') {
    if (!flag_state->c_flag && !flag_state->l_flag) flag_state->n_flag = 1;
  } else if (option == 'h') {
    flag_state->h_flag = 1;
  } else if (option == 's') {
    flag_state->s_flag = 1;
  } else if (option == 'f') {
    flag_state->f_flag = 1;
    dynamic_array = add_list_db(dynamic_array, optarg);
  } else if (option == 'o') {
    if (!flag_state->v_flag && !flag_state->c_flag && !flag_state->l_flag)
      flag_state->o_flag = 1;
  } else if (option == '?') {
    flag_state->err_flag = 1;
  }
}

void grep(program_flags flag_state, char** arguments, FILE* fp,
          char* file_name) {
  cycle_flags cycle_state = {0};
  regex_t regex;
  regmatch_t match;
  char* line = NULL;
  int line_num = 1;
  int prev_line_num = 0;
  ssize_t ch = 0;
  size_t n = 0;
  while ((ch = getline(&line, &n, fp)) != -1) {
    while (ch > 0 && (line[ch - 1] == '\r' || line[ch - 1] == '\n'))
      line[--ch] = 0;
    char* line_ptr = line;
    cycle_state.line_match = 0;
    cycle_state.v_matches = 0;
    for (int i = 0; arguments[i]; i++) {
      int ret =
          regcomp(&regex, arguments[i], flag_state.i_flag ? REG_ICASE : 0);
      if (!ret)
        ret = regexec(&regex, line_ptr, 1, &match, 0);
      else {
        char* pre_ret = flag_state.i_flag ? my_strcasestr(line, arguments[i])
                                          : strstr(line, arguments[i]);
        ret = pre_ret ? 0 : 1;
      }
      if (!cycle_state.line_match) {
        if (!flag_state.v_flag && !ret) {
          print_prefixes(flag_state, file_name, &prev_line_num, line_num);
          if (flag_state.o_flag)
            print_o_matches(line, &regex, &match);
          else
            get_line_match(flag_state, &cycle_state, line);
        } else if (flag_state.v_flag && ret) {
          cycle_state.v_matches++;
          if (cycle_state.v_matches == arrind) {
            print_prefixes(flag_state, file_name, &prev_line_num, line_num);
            get_line_match(flag_state, &cycle_state, line);
          }
        }
      }
      regfree(&regex);
    }
    line_num++;
  }
  print_cl_matches(flag_state, cycle_state, file_name);
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

// Create because strcasestr() is not specified by POSIX.1-2017
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

void print_prefixes(program_flags flag_state, char* file_name,
                    int* prev_line_num, int line_num) {
  if (*prev_line_num != line_num) {
    if (flag_state.multiple_files && !flag_state.h_flag && !flag_state.c_flag)
      printf("%s:", file_name);
    if (flag_state.n_flag) printf("%d:", line_num);
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

void get_line_match(program_flags flag_state, cycle_flags* cycle_state,
                    char* line) {
  if (flag_state.c_flag)
    if (!cycle_state->l_match) cycle_state->c_matches++;
  if (flag_state.l_flag) cycle_state->l_match = 1;
  if (!flag_state.o_flag && !flag_state.c_flag && !flag_state.l_flag)
    printf("%s\n", line);
  cycle_state->line_match = 1;
}

void print_cl_matches(program_flags flag_state, cycle_flags cycle_state,
                      char* file_name) {
  if (flag_state.c_flag) {
    if (flag_state.multiple_files && !flag_state.h_flag)
      printf("%s:", file_name);
    printf("%d\n", cycle_state.c_matches);
  }
  if (flag_state.l_flag && cycle_state.l_match) printf("%s\n", file_name);
}