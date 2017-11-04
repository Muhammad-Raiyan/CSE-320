#include "executable.h"

void sigchld_handler(int s){
    int olderrno = errno;
    pid = waitpid(-1, NULL, 0);
    errno = olderrno;
}

void sigint_handler(int s){

}

bool start_exec(cmd* c){
    set_cmd_IO(c);
    //BUILTIN HANDLE
    if(get_builtin_code(c->argv[0])!=-1){
        call_builtin(c);

        return true;
    }

    if(pLength > 1){
        initPipe(c);
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

void initPipe(cmd* head){
    cmd *current = head;
    int i = 0;
    int status;
    int numPipes = 2*pLength;
    int fd[numPipes];

    for(i = 0; i<numPipes; i++){
        Pipe(fd+i*2);
    }

    /*int* temp_pipes = fd;
    current->out = temp_pipes[1];
    while(current->next!=NULL){
        current = current->next;
        current->in = temp_pipes[0];

        if(current->next == NULL) break;
        temp_pipes+=2;
        current->out = temp_pipes[1];
    }*/
    int j = 0;
    while(current){
        //if not last cmd
        pid = fork();

        if(pid == 0){
            if(current->next){
                if(dup2(fd[j + 1], 1) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            if(j!= 0){
                if(dup2(fd[j-2], 0) < 0){
                    perror(" dup2");///j-2 0 j+1 1
                    exit(EXIT_FAILURE);
                }
            }
            for(i = 0; i < 2*numPipes; i++){
                close(fd[i]);
            }

            if( execvp(current->argv[0], current->argv) < 0 ){
                perror(EXEC_ERROR);
                exit(EXIT_FAILURE);
            }
        } else if(pid < 0){
            perror("error");
            exit(EXIT_FAILURE);
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