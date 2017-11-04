#include "sfish.h"


int n_argc = 0;
char **n_argv = NULL;

int main(int argc, char *argv[], char* envp[]) {
    char* input = NULL;
    keepRunning = 1;
    if(!isatty(STDIN_FILENO)) {
        // If your shell is reading from a piped file
        // Don't have readline write anything to that file.
        // Such as the prompt or "user input"
        if((rl_outstream = fopen("/dev/null", "w")) == NULL){
            perror("Failed trying to open DEVNULL");
            exit(EXIT_FAILURE);
        }
    }

    bool running = true;
    do {
        char *prompt = get_prompt();
        input = readline(prompt);
        if(input == NULL || strcmp(input, "")==0) {
            continue;
        }
        n_argv = set_arguments(input, &n_argc);
        cmd* c = calloc(MAXARG, sizeof(cmd));
        pLength = 0;
        c = parse_input(input);
        //debug("PLength: %d", pLength);
        running = start_exec(c);

        rl_free(input);
        free(n_argv);
        free(prompt);

    } while(running);

    debug("%s", "user entered 'exit'");

    return EXIT_SUCCESS;
}
