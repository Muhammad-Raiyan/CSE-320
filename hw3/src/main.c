#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {


    sf_mem_init();
    /* void *w = */ sf_malloc(sizeof(long));
    void *x = sf_malloc(sizeof(double) * 11);
    void *y = sf_malloc(sizeof(char));
    /* void *z = */ sf_malloc(sizeof(int));

    sf_free(y);
    sf_free(x);
    sf_snapshot();
    sf_mem_fini();

    return EXIT_SUCCESS;
}
