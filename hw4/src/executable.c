#include "executable.h"

void sigchld_handler(int s){
    int olderrno = errno;
    pid = waitpid(-1, NULL, 0);
    errno = olderrno;
}

void sigint_handler(int s){

}

bool start_exec(cmd* c){
    set_pipe_IO(c);
    //BUILTIN HANDLE
    if(get_builtin_code(c->argv[0])!=-1){
        call_builtin(c);

        return true;
    }
    // EXECUTABLES HANDLE
    sigset_t mask, prev;
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Sigemptyset(&mask);
    Sigaddset(&mask, SIGCHLD);

    Sigprocmask(SIG_BLOCK, &mask, &prev);   // BLOCK SIGCHLD
    if(Fork()==0){
        Sigprocmask(SIG_SETMASK, &prev, NULL);
        //char **argv = c->argv;

        run(c);
    }

    pid = 0;

    while(!pid){
        Sigsuspend(&prev);
    }

    Sigprocmask(SIG_SETMASK, &prev, NULL);
    return true;
}

void set_pipe_IO(cmd* head){
    cmd *current = head;
    if(current->next == NULL)
        set_cmd_IO(current);
    else {
        do {
            set_cmd_IO(current);
        } while((current=current->next)!= NULL);
    }

}

void set_cmd_IO(cmd* c){
    int pos;

    if((pos=has_left_redirect(c->argc, c->argv))!=-1){
        char fileName[MAXARG];
        memset(fileName, '\0', MAXARG);
        strcpy(fileName, c->argv[pos+1]);
        c->argv[pos] = NULL;
        int fid = Open(fileName, O_RDONLY);
        c->in = fid;
        /*Dup2(c->in, STDIN_FILENO);
        close(fid);*/
    }

    if((pos=has_right_redirect(c->argc, c->argv))!=-1){
        char fileName[MAXARG];
        memset(fileName, '\0', MAXARG);
        strcpy(fileName, c->argv[pos+1]);
        c->argv[pos] = NULL;
        int fid = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        c->out = fid;
        /*Dup2(c->out, STDOUT_FILENO);
        close(fid);*/
    }
}

void run(cmd* c){

    Dup2(c->in, STDIN_FILENO);
    Dup2(c->out, STDOUT_FILENO);
    if(c->in!=STDIN_FILENO)close(c->in);
    if(c->out!=STDOUT_FILENO)close(c->out);
    Execvp(c->argv[0], c->argv);
}