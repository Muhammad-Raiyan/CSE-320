
#include "sfmm_helpers.h"

bool needs_new_page(){
    return get_heap_start() == NULL ? true : false;
}

void set_header_bits(sf_header *header, bool allocate, bool paddded, size_t blk_size){

    header -> allocated = 1;
    header -> padded = 0;
    header -> two_zeroes = 0;
    header -> block_size = blk_size;
    header -> unused =0;

}

void set_footer_bits(sf_footer *footer,  bool allocate, bool padded, size_t blk_size, size_t req_size){

    footer -> allocated = 1;
    footer -> padded = 0;
    footer -> two_zeroes = 0;
    footer -> block_size = blk_size;
    footer -> requested_size =req_size;

}