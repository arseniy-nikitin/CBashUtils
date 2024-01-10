#include "dynamic_db.h"

int arrind = 0;
size_t maxl = MAXL;

char** init_db(char** array) {
  array = calloc(maxl, sizeof(*array));
  if (!array) fprintf(stderr, "init_db: unble to allocate virtual memory\n");
  return array;
}

char** add_single_db(char** array, char* var) {
  if (var) {
    array[arrind++] = strdup(var);
    if (arrind == (int)maxl) array = resize_db(array);
  }
  return array;
}

char** add_list_db(char** array, char* file_name) {
  FILE* fp = fopen(file_name, "r");
  if (fp) {
    char* line = NULL;
    ssize_t ch = 0;
    size_t n = 0;
    while ((ch = getline(&line, &n, fp)) != -1) {
      while (ch > 0 && (line[ch - 1] == '\r' || line[ch - 1] == '\n'))
        line[--ch] = '\0';
      array = add_single_db(array, line);
    }
    if (line) free(line);
  } else
    fprintf(stderr, "add_list_db: unble to open a file\n");
  fclose(fp);
  return array;
}

char** resize_db(char** array) {
  char** arrptr = array;
  char** tmp = realloc(arrptr, 2 * maxl * sizeof(*tmp));
  if (tmp) {
    arrptr = tmp;
    memset(arrptr + maxl, 0, maxl * sizeof(*tmp));
    maxl *= 2;
  } else
    fprintf(stderr, "resize_db: unble to allocate virtual memory\n");
  return arrptr;
}

void free_db(char** array) {
  for (int i = 0; i < arrind; i++)
    if (array[i]) free(array[i]);
  if (array) free(array);
  maxl = MAXL;
  arrind = 0;
}