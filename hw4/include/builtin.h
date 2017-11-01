#ifndef BUILTIN
#define BUILTIN

#include "util.h"

const char *builtinList[4];
const int builtinCount;
char OLDPWD[256];

bool is_builtin(const char *arg);
void call_builtin(int argc, char *argv[]);
int get_builtin_code(const char *arg);
void help();
void my_exit();
void cd(const char *path);
void pwd();

#endif