#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {

    sf_mem_init();
    void *x = sf_malloc(sizeof(double) * 8);
    void *y = sf_realloc(x, sizeof(int));
    (void ) y;
    sf_snapshot();
    sf_mem_fini();

    return EXIT_SUCCESS;
}
