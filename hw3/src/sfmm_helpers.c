
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

sf_free_header *getFreeBlock(sf_free_header *headNode, size_t size){
    sf_free_header *node = headNode;
    while(node != NULL){
        size_t payload_size = node->header.block_size << 4;
        if(payload_size > size)
            return node;
        node = node->next;
    }

    return NULL;
}

sf_header *getFreeBlockHeader(sf_free_header *list, size_t size){
    sf_free_header *node = list;
    while(node != NULL){
        size_t payload_size = node->header.block_size << 4;
        if(payload_size > size)
            return &node->header;
        node = node->next;
    }

    return NULL;
}

sf_footer *getBlockFooter(sf_header *header){
    size_t blockSize = header->block_size;

    sf_footer *footer = (sf_footer *)((char *)header + (blockSize<<4) -8);
    return footer;
}

size_t get_padded_size(size_t size){
    if(size%16 == 0) {
        return size;
    }
    else {
        while(size%16!=0){
            size++;
        }
    }
    return size;
}

sf_header *getNewFreeHeader(sf_header *allocatedHeader, size_t oldSize, size_t reqSize){
    size_t paddedSize = get_padded_size(reqSize);
    size_t freeBlockSize = oldSize - (paddedSize+16);
    //size_t freeBlockSize = freePldSize + 16;

    sf_footer *allocatedFooterPtr = (sf_footer *)((char *) allocatedHeader + paddedSize + 8);
    char *freeHeaderPtr = (char*)allocatedFooterPtr+8;
    sf_footer *oldFreeFooter =(sf_footer *)(freeHeaderPtr + freeBlockSize - 8);


    sf_header *newFreeHeader = set_header_bits((sf_header *)freeHeaderPtr, false, false, freeBlockSize);
    set_footer_bits(oldFreeFooter, false, false, freeBlockSize, freeBlockSize);
    sf_blockprint(newFreeHeader);
    //sf_varprint(newFreeHeader);
    return newFreeHeader;
}
/*
debug("allocatedHeader %p\n", allocatedHeader);
    debug("footerPtr %p\n", footerPtr);
    debug("Diff of alloc block %ld\n", (char *)footerPtr - (char *)allocatedHeader);
    debug("freeHeaderPtr %p\n", freeHeaderPtr);
    debug("oldFreeFooter %p\n", oldFreeFooter);
    debug("Diff of header %ld\n", freeHeaderPtr - (char *)allocatedHeader);
    debug("FREE BLOCK %ld", freeBlockSize);
    debug("Diff %ld", (char *)oldFreeFooter - (char *)freeHeaderPtr);
    debug("Total diff %ld", (char *)oldFreeFooter-(char *)allocatedHeader );*/