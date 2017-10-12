
#include "sfmm_helpers.h"


void* set_header_bits(sf_header *header, bool alloc, bool padded, size_t blk_size){

    if(alloc == true) header -> allocated = 1;
    else header -> allocated = 0;

    if(padded == true) header -> padded = 1;
    else header -> padded = 0;

    header -> two_zeroes = 0;
    header -> block_size = blk_size >> 4;
    header -> unused = 0;

    return header;
}

void set_footer_bits(sf_footer *footer,  bool alloc, bool padded, size_t blk_size, size_t req_size){
    if(alloc == true) footer -> allocated = 1;
    else footer -> allocated = 0;

    if(padded == true) footer -> padded = 1;
    else footer -> padded = 0;

    footer -> two_zeroes = 0;
    footer -> block_size = blk_size >> 4;
    footer -> requested_size =req_size;

}


sf_free_header *get_seg_free_list_header(free_list *list, size_t size){

    if(size <LIST_1_MAX && hasFreeBlock(list[0], size)){
        return list[0].head;
    }
    if(size <LIST_2_MAX && hasFreeBlock(list[1], size)){
        return list[1].head;
    }
    if(size <LIST_3_MAX && hasFreeBlock(list[2], size)){
        return list[2].head;
    }
    if(size <LIST_4_MAX && hasFreeBlock(list[3], size)){
        return list[3].head;
    }
    return NULL;
}

bool hasFreeBlock(free_list list, size_t size){
    sf_free_header *node = list.head;
    while(node != NULL){
        size_t payload_size = node->header.block_size << 4;
        if(payload_size > size)
            return true;
        node = node->next;
    }

    return false;
}

/*void *getFreeBlock(free_list list, size_t size){
    sf_free_header *node = list.head;
    while(node != NULL){
        size_t payload_size = node->header.block_size << 4;
        if(payload_size > size)
            return (void *) node.header;
        node = node->next;
    }

    return NULL;
}*/