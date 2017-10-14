#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {


    sf_mem_init();
    sf_snapshot();
    double *ptr = sf_malloc(40);
    sf_blockprint((char *)ptr-8);
    sf_snapshot();

    /*double *ptr4 = sf_malloc(512);
    sf_blockprint((char *)ptr4-8);*/
    //*ptr = 320320320e-320;

    //printf("%f\n", *ptr);
    //sf_snapshot();
    //sf_varprint(ptr);
    //sf_free(ptr);

    sf_mem_fini();

    return EXIT_SUCCESS;
}
