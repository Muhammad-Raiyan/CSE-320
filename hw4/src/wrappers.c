#include "wrappers.h"

ssize_t sio_puts(char s[]) /* Put string */
{
    return write(STDOUT_FILENO, s, strlen(s)); //line:csapp:siostrlen
}

void unix_error(char *msg) {
    char *errMsg = malloc(256 + strlen(msg));
    strcpy(errMsg, msg);
    strcat(errMsg, strerror(errno));
    strcat(errMsg, "\n");
    sio_puts(errMsg);
   //fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}

int Chdir(const char* path){
    int ret = chdir(path);

    if(ret != 0 ){
        char *msg = malloc(strlen(BUILTIN_ERROR)+strlen(path) + 4);

        strcpy(msg, BUILTIN_ERROR);
        strcat(msg, "cd: ");
        strcat(msg, path);
        strcat(msg, ": ");
        unix_error(msg);
        free(msg);
    }
    return ret;
}

pid_t Fork(){
    //pid_t pid;

    if ((pid = fork()) < 0){
        char errorMsg[256];
        strcat(errorMsg, EXEC_ERROR);
        strcat(errorMsg, "Fork");
        unix_error(errorMsg);
    }
    return pid;

}

pid_t Waitpid(pid_t pid, int *iptr, int options)
{
    pid_t retpid;

    if ((retpid  = waitpid(pid, iptr, options)) < 0){
        unix_error("Waitpid error");
    }
    return(retpid);
}

handler_t *Signal(int signum, handler_t *handler){
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
    unix_error("Signal error");
    return (old_action.sa_handler);
}
/* $end sigaction */

void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset){
    if (sigprocmask(how, set, oldset) < 0)
        unix_error("Sigprocmask error");
    return;
}

void Sigemptyset(sigset_t *set)
{
    if (sigemptyset(set) < 0)
        unix_error("Sigemptyset error");
    return;
}

void Sigfillset(sigset_t *set)
{
    if (sigfillset(set) < 0)
        unix_error("Sigfillset error");
    return;
}

void Sigaddset(sigset_t *set, int signum)
{
    if (sigaddset(set, signum) < 0)
        unix_error("Sigaddset error");
    return;
}

void Sigdelset(sigset_t *set, int signum)
{
    if (sigdelset(set, signum) < 0)
        unix_error("Sigdelset error");
    return;
}

int Sigismember(const sigset_t *set, int signum)
{
    int rc;
    if ((rc = sigismember(set, signum)) < 0)
        unix_error("Sigismember error");
    return rc;
}

int Sigsuspend(const sigset_t *set)
{
    int rc = sigsuspend(set); /* always returns -1 */
    if (errno != EINTR)
        unix_error("Sigsuspend error");
    return rc;
}

void Execvp(const char *filename, char *const argv[])
{
    if (execvp(filename, argv) < 0){
        unix_error("Execve error");
        _exit(0);
    }
}

