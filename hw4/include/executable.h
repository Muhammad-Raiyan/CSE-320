#ifndef EXECUTABLE
#define EXECUTABLE

#include "sfish.h"

void sigchld_handler(int s);
void sigint_handler(int s);
bool execute(char **argv);

#endif