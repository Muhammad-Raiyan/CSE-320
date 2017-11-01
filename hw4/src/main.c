#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <util.h>
#include "builtin.h"
#include "sfish.h"
#include "debug.h"


int n_argc = 0;
char **n_argv = NULL;

int main(int argc, char *argv[], char* envp[]) {
    char* input;
    bool exited = false;

    if(!isatty(STDIN_FILENO)) {
        // If your shell is reading from a piped file
        // Don't have readline write anything to that file.
        // Such as the prompt or "user input"
        if((rl_outstream = fopen("/dev/null", "w")) == NULL){
            perror("Failed trying to open DEVNULL");
            exit(EXIT_FAILURE);
        }
    }

    do {
        char *prompt = get_prompt();
        input = readline(prompt);

        write(1, "\e[s", strlen("\e[s"));
        write(1, "\e[20;10H", strlen("\e[20;10H"));
        write(1, "SomeText", strlen("SomeText"));
        write(1, "\e[u", strlen("\e[u"));

        // If EOF is read (aka ^D) readline returns NULL
        if(input == NULL) {
            continue;
        }
        char *input_cpy = malloc(strlen(input));
        strcpy(input_cpy, input);
        n_argv = set_arguments(input_cpy, &n_argc);
        debug("after %s", input);

        if(get_builtin_code(n_argv[0])!=-1){
            call_builtin(n_argc, n_argv);
        }
        else
            printf(EXEC_NOT_FOUND, input);

        // You should change exit to a "builtin" for your hw.
        exited = strcmp(input, "exit") == 0;
        debug("after call_builtin %s", input);
        // Readline mallocs the space for input. You must free it.
        rl_free(input);
        free(n_argv);
        free(prompt);

    } while(!exited);

    debug("%s", "user entered 'exit'");

    return EXIT_SUCCESS;
}
