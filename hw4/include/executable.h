#ifndef EXECUTABLE
#define EXECUTABLE

#include "sfish.h"

void sigchld_handler(int s);
void sigint_handler(int s);
void execute(char **argv);

#endif