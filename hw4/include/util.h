#ifndef UTIL
#define UTIL

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "debug.h"

char** set_arguments(char *input, int *argc);
char* get_prompt();

#endif