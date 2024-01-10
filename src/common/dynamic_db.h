#ifndef DYNAMIC_DB
#define DYNAMIC_DB

#define _POSIX_C_SOURCE 200809L

#define MAXL 16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

extern int arrind;
extern size_t maxl;

char** init_db(char** array);
char** add_single_db(char** array, char* var);
char** add_list_db(char** array, char* file_name);
char** resize_db(char** array);
void free_db(char** array);

#endif