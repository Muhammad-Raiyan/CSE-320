#ifndef WRAPPERS
#define WRAPPERS

#include "sfish.h"

void unix_error(char *msg);

int Chdir(const char* path);
pid_t Fork();
handler_t *Signal(int signum, handler_t *handler);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
void Sigemptyset(sigset_t *set);
void Sigfillset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);
void Sigdelset(sigset_t *set, int signum);
int Sigismember(const sigset_t *set, int signum);
int Sigsuspend(const sigset_t *set);
ssize_t sio_puts(char s[]);

void Execvp(const char *filename, char *const argv[]);

#endif
