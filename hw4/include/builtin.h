#ifndef BUILTIN
#define BUILTIN

#include "sfish.h"

const char *builtinList[4];
const int builtinCount;
char OLDPWD[256];

bool is_builtin(const char *arg);
void call_builtin(cmd* c);
int get_builtin_code(const char *arg);
void help(cmd* c);
void Exit();
void cd(const char *path);
void pwd(cmd* c);

#endif