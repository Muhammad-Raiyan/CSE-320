#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {


    sf_mem_init();

    double *ptr = sf_malloc(sizeof(double));
    sf_blockprint(ptr);
    //*ptr = 320320320e-320;

    //printf("%f\n", *ptr);
    sf_snapshot();
    //sf_varprint(ptr);
    sf_free(ptr);

    sf_mem_fini();

    return EXIT_SUCCESS;
}
