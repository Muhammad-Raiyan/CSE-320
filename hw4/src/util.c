#include "util.h"

char** set_arguments(char *input, int *argc){

    char *token = NULL, *temp = input;
    char **argv = malloc(strlen(input));
    int i = 0;
    while((token = strtok_r(temp, " \r\t\n", &temp))){
        if(strcmp(token, "\r")!=0 && strcmp(token, "\t")!=0 && strcmp(token, "\n")!=0){
            argv[i] = token;
            i++;
        }
    }
    argv[i] = '\0';
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

    debug("Home: %s", home);
    debug("CWD: %s", cwd);
    strcat(cwd_start, netid);
    return cwd_start;
}