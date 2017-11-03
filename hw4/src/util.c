#include "util.h"

char** set_arguments(char *input, int *argc){

    char *token = NULL, *temp = input;
    char **argv = calloc(strlen(input), sizeof(input));

    int i = 0;
    while((token = strtok_r(temp, " \r\t\n", &temp))){
            argv[i] = token;
            i++;
    }
    //argv[i] = '\0';
    *argc = i;
    return argv;
}

char* get_prompt(){
    const char *netid = " :: mrislam >> ";
    char *home = getenv("HOME");
    char cwd[256];
    getcwd(cwd, sizeof(cwd));

    char *cwd_start = calloc(strlen(cwd)+strlen(netid), sizeof(char));
    if(strncmp(cwd, home, strlen(home)) == 0){
        strcpy(cwd_start, "~");
        strcat(cwd_start, cwd+strlen(home));
    }
    else strcpy(cwd_start, cwd);

    //debug("Home: %s", home);
    //debug("CWD: %s", cwd);
    strcat(cwd_start, netid);
    return cwd_start;
}

cmd* parse_input(char* input){
    cmd* c = calloc(1, sizeof(cmd));
    c->original = input;

    //DEFAULT I/O
    c->in =STDIN_FILENO;
    c->out = STDOUT_FILENO;

    /*char** t_argv =set_arguments(input, &(c->argc));
    *(c->argv) = t_argv;*/
    //char** t_argv = c->argv;
    char **t_argv = set_arguments(input, &(c->argc));

    for(int i=0; i<c->argc; i++){

        c->argv[i] = t_argv[i];
    }
    return c;
}