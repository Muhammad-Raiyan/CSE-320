#ifndef EXECUTABLE
#define EXECUTABLE

#include "sfish.h"

void sigchld_handler(int s);
void sigint_handler(int s);

int start_exec(cmd* c);
void run(cmd* c);

void runPipe(cmd* c);
void set_cmd_IO(cmd* c);

void Pipe( int* arr );
#endif