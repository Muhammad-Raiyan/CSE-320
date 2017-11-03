#ifndef EXECUTABLE
#define EXECUTABLE

#include "sfish.h"

void sigchld_handler(int s);
void sigint_handler(int s);
bool execute(cmd* c);

cmd* set_cmd_IO(cmd* c);
#endif