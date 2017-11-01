#include "wrappers.h"

void unix_error(char *msg, bool doExit) {
   fprintf(stderr, "%s: %s\n", msg, strerror(errno));
   if(doExit)
        exit(0);
}

int Chdir(const char* path){
    int ret = chdir(path);

    if(ret != 0 ){
        char *msg = malloc(strlen(BUILTIN_ERROR)+strlen(path) + 4);
        char *errorArg = malloc(4+strlen(path));

        strcpy(errorArg, "cd: ");
        strcat(errorArg, path);
        sprintf(msg, BUILTIN_ERROR, errorArg);
        unix_error(msg, false);
        free(msg);
        free(errorArg);
    }
    return ret;
}