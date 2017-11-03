#ifndef UTIL
#define UTIL

#include "sfish.h"

char** set_arguments(char *input, int *argc);
cmd* parse_input(char *input);
char* get_prompt();

#endif