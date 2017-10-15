#include <stdio.h>
#include "sfmm.h"
#include "sfmm_helpers.h"

int main(int argc, char const *argv[]) {

    sf_mem_init();
    void *x = sf_malloc(48);

    sf_header *hed = (sf_header *)((char *) x -8);
    sf_footer *foot = (sf_footer *)((char *) hed + (hed->block_size << 4) - SF_FOOTER_SIZE/8);
    sf_blockprint(hed);
    (void) foot;
    return EXIT_SUCCESS;
}
