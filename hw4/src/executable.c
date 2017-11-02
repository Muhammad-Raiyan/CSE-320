#include "executable.h"

void sigchld_handler(int s){
    int olderrno = errno;
    pid = waitpid(-1, NULL, 0);
    errno = olderrno;
}

void sigint_handler(int s){

}

void execute(char **argv){
    sigset_t mask, prev;
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Sigemptyset(&mask);
    Sigaddset(&mask, SIGCHLD);

    Sigprocmask(SIG_BLOCK, &mask, &prev);   // BLOCK SIGCHLD
    if(Fork()==0){
        Sigprocmask(SIG_SETMASK, &prev, NULL);
        Execvp(argv[0], argv);
    }

    pid = 0;

    /*while ((pid = waitpid(-1, NULL, 0)) > 0){
            sio_puts("Hello\n");
    };*/

    while(!pid){
        Sigsuspend(&prev);
    }

    Sigprocmask(SIG_SETMASK, &prev, NULL);
}