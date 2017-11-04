#include "executable.h"

void sigchld_handler(int s){
    int olderrno = errno;
    pid = waitpid(-1, NULL, 0);
    errno = olderrno;
}

void sigint_handler(int s){
    printf("Pressed ctrl-c?\n");
    keepRunning = 0;
}

void sigstp_handler(int s){
    kill(pid, SIGTSTP);
}

bool start_exec(cmd* c){
    set_cmd_IO(c);
    //BUILTIN HANDLE
    if(get_builtin_code(c->argv[0])!=-1){
        call_builtin(c);

        return true;
    }

    if(pLength > 1){
        runPipe(c);
        return true;
    }

    // EXECUTABLES HANDLE
    sigset_t mask, prev;
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigstp_handler);
    Sigemptyset(&mask);
    Sigaddset(&mask, SIGCHLD);

    Sigprocmask(SIG_BLOCK, &mask, &prev);   // BLOCK SIGCHLD
    if(Fork()==0){
        Sigprocmask(SIG_SETMASK, &prev, NULL);
        run(c);
    }

    pid = 0;

    while(!pid){
        Sigsuspend(&prev);
    }

    Sigprocmask(SIG_SETMASK, &prev, NULL);
    return true;
}

void runPipe(cmd* head){
    cmd *current = head;
    int i = 0;
    int status;
    int numPipes = 2*pLength;
    int fd[numPipes];

    for(i = 0; i<numPipes; i++){
        Pipe(fd+i*2);
    }

    int j = 0;
    while(current){
        //if not last cmd
        pid = Fork();

        if(pid == 0){
            if(current->next){
                Dup2(fd[j + 1], 1);
            }
            if(j!= 0){
                Dup2(fd[j-2], 0);
            }
            for(i = 0; i < 2*numPipes; i++){
                close(fd[i]);
            }
            Execvp(current->argv[0], current->argv);
        }
        current = current->next;
        j+= 2;
    }

    for(i = 0; i < 2 * numPipes; i++){
            close(fd[i]);
    }

    for(i = 0; i < numPipes + 1; i++)
            wait(&status);
}

void set_cmd_IO(cmd* c){

    cmd *current = c;
    do{
        int pos;

        if((pos=has_left_redirect(current->argc, current->argv))!=-1){
            char fileName[MAXARG];
            memset(fileName, '\0', MAXARG);
            strcpy(fileName, current->argv[pos+1]);
            current->argv[pos] = NULL;
            int fid = Open(fileName, O_RDONLY);
            current->in = fid;
            /*Dup2(c->in, STDIN_FILENO);
            close(fid);*/
        }

        if((pos=has_right_redirect(current->argc, current->argv))!=-1){
            char fileName[MAXARG];
            memset(fileName, '\0', MAXARG);
            strcpy(fileName, current->argv[pos+1]);
            current->argv[pos] = NULL;
            int fid = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            current->out = fid;
            /*Dup2(c->out, STDOUT_FILENO);
            close(fid);*/
        }
    } while((current=current->next)!=NULL);

}

void run(cmd* c){

    Dup2(c->in, STDIN_FILENO);
    Dup2(c->out, STDOUT_FILENO);
    if(c->in!=STDIN_FILENO)close(c->in);
    if(c->out!=STDOUT_FILENO)close(c->out);
    Execvp(c->argv[0], c->argv);
}