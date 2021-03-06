#ifndef UTIL
#define UTIL

#include "sfish.h"

char** set_arguments(char *input, int *argc);
cmd* parse_input(char *input);
char* get_prompt();

int has_left_redirect(int argc, char** argv);
int has_right_redirect(int argc, char** argv);

cmd* initCommands(char* input);
void addCmd(cmd* head, char* val);
void parse_cmd(cmd* head);
#endif