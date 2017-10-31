
#include "util.h"

char** setArguments(char *input, int *argc){
    /* TODO: HAVE NOT FREED mrgv */
    char *token = NULL, *temp = input;
    char **argv = malloc(strlen(input));
    int i = 0;
    while((token = strtok_r(temp, " ", &temp))){
        argv[i] = token;
        i++;
    }
    *argc = i;
    return argv;
}

char* getPrompt(){
    const char *netid = " :: mrislam >> ";
    char *home = getenv("HOME");
    char *cwd = getenv("PWD");
    char *cwd_start = calloc(strlen(cwd)+strlen(netid), sizeof(char));
    if(strncmp(cwd, home, strlen(home)) == 0){
        strcpy(cwd_start, "~");
        strcat(cwd_start, cwd+strlen(home));
    }
    else strcpy(cwd_start, cwd);

    debug("Home: %s\n", home);
    debug("CWD: %s\n", cwd);
    strcat(cwd_start, netid);
    return cwd_start;
}