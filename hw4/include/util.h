#ifndef UTIL
#define UTIL

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <errno.h>
#include "debug.h"
#include "sfish.h"

char** set_arguments(char *input, int *argc);
char* get_prompt();

#endif