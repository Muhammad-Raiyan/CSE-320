#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {


    sf_mem_init();
    sf_malloc(sizeof(long));
    void *x = sf_malloc(sizeof(double) * 11);
    void *y = sf_malloc(sizeof(char));
    sf_malloc(sizeof(int));
    sf_snapshot();
    sf_free(y);
    sf_snapshot();
    printf("3\n");
    sf_free(x);
    sf_snapshot();
    printf("4\n");
    sf_mem_fini();

    return EXIT_SUCCESS;
}
