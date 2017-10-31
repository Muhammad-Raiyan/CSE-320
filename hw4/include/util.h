#ifndef UTIL
#define UTIL

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "debug.h"

char** setArguments(char *input, int *argc);
char* getPrompt();
#endif