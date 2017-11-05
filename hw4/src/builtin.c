#include "builtin.h"

const char *builtinList[4] = {"help", "exit", "cd", "pwd"};
const int builtinCount = 4;


bool is_builtin(const char* arg){

    for(int i = 0; i<builtinCount; i++){
        if(strcmp(arg, builtinList[i])==0)
            return true;
    }
    return false;
}

void call_builtin(cmd* c){
    int code = get_builtin_code(c->argv[0]);
    switch(code){
        case 0: help(c); break;
        case 1: Exit(); break;
        case 2: cd(c->argv[1]); break;
        case 3: pwd(c); break;
        default:
            perror(BUILTIN_ERROR);
            break;
    }
}

int get_builtin_code(const char *arg){
    for(int i = 0; i<builtinCount; i++){
        if(strcmp(arg, builtinList[i])==0)
            return i;
    }
    return -1;
}

void help(cmd* c){
    for(int i=0; i<builtinCount; i++){
        char *s = calloc(strlen(builtinList[i]), sizeof(char));
        sprintf(s, JOBS_LIST_ITEM, i, builtinList[i]);
        write(c->out, s, strlen(s));
    }
}

void Exit(){
    //kill(pid, SIGTERM);
    exit(0);
}

void cd(const char *path){
    char pwd_before_cd[256];
    char *temp_path = malloc(256);

    if(path!=NULL){
        strcpy(temp_path, path);
    }
    else {
        strcpy((char *)temp_path, getenv("HOME"));
        //debug("evg: %s", getenv("HOME"));
    }
    getcwd(pwd_before_cd, sizeof(pwd_before_cd));

    if(strcmp(temp_path, "-")==0){
        strcpy((char *)temp_path, OLDPWD);
    }
    //debug("%s", temp_path);
    int ret = Chdir(temp_path);

    if(ret == 0){
        strcpy(OLDPWD, pwd_before_cd);
    }
}

void pwd(cmd* c){
    char cwd[256];

    if(getcwd(cwd, sizeof(cwd))==NULL){
        perror("Error getcwd");
    }
    else {
        strcat(cwd, "\n");
        write(c->out, cwd, strlen(cwd));
    }

}

void set_color(char* chr){

}
