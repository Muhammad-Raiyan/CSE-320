#include "util.h"

char** set_arguments(char *input, int *argc){

    char *token = NULL, *temp = malloc(strlen(input));
    char **argv = calloc(strlen(input), sizeof(input));
    strcpy(temp, input);
    int i = 0;
    while((token = strtok_r(temp, DELIM, &temp))){
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
    c = initCommands(input);

    parse_cmd(c);
    //DEFAULT I/O
    /*c->in =STDIN_FILENO;
    c->out = STDOUT_FILENO;

    char **t_argv = set_arguments(input, &(c->argc));

    for(int i=0; i<c->argc; i++){
        c->argv[i] = t_argv[i];
    }*/
    return c;
}

cmd* initCommands(char* input){
    char *token = NULL, *temp = malloc(strlen(input));
    strcpy(temp, input);
    //char **argv = calloc(strlen(input), sizeof(input));
    cmd* c = NULL;
    c = calloc(1, sizeof(cmd));
    c->next = NULL;

    while((token = strtok_r(temp, "|", &temp))){
        addCmd(c, token);
    }
    c = c->next;
    return c;
}

void addCmd(cmd* head, char* val){
    cmd* current = head;
    while(current->next!=NULL){
        current = current->next;
    }

    current->next = calloc(1, sizeof(cmd));
    current->next->original = calloc(strlen(val), sizeof(char));
    strcpy(current->next->original, val);
    current->next->next = NULL;
}

void parse_cmd(cmd* head){
    cmd* current = head;

    do{
        current->in =STDIN_FILENO;
        current->out = STDOUT_FILENO;

        char **t_argv = set_arguments(current->original, &(current->argc));

        for(int i=0; i<current->argc; i++){
            current->argv[i] = t_argv[i];
        }


    } while((current = current->next)!=NULL);
}

int has_left_redirect(int argc, char** argv){

    for(int i=0; i<argc; i++){
        if(argv[i] != NULL && strcmp(argv[i], "<")==0){
            return i;
        }
    }
    return -1;
}

int has_right_redirect(int argc, char** argv){

    for(int i=0; i<argc; i++){
        if(argv[i] != NULL && strcmp(argv[i], ">")==0){
            return i;
        }
    }
    return -1;
}

