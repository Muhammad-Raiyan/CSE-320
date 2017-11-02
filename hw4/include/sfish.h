#ifndef SFISH_H
#define SFISH_H
#define _GNU_SOURCE

/* Format Strings */
#define EXEC_NOT_FOUND "sfish: %s: command not found\n"
#define JOBS_LIST_ITEM "[%d] %s\n"
#define STRFTIME_RPRMT "%a %b %e, %I:%M%p"
#define BUILTIN_ERROR  "sfish builtin error: "
#define SYNTAX_ERROR   "sfish syntax error: %s"
#define EXEC_ERROR     "sfish exec error: %s"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

typedef void handler_t(int);
volatile sig_atomic_t pid;

#include "wrappers.h"
#include "util.h"
#include "debug.h"
#include "builtin.h"
#include "executable.h"

#endif
