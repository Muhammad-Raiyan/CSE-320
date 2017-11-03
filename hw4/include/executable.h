#ifndef EXECUTABLE
#define EXECUTABLE

#include "sfish.h"

void sigchld_handler(int s);
void sigint_handler(int s);
bool start_exec(cmd* c);

void set_cmd_IO(cmd* c);
#endif