
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
    footer -> requested_size = req_size;

}


sf_free_header *get_seg_free_list_head(free_list *list, size_t size){

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

sf_header *getNewFreeHeader(sf_header *allocatedHeader, size_t oldSize, size_t reqSize){
    size_t paddedSize = get_padded_size(reqSize);
    size_t freeBlockSize = oldSize - (paddedSize+16);

    sf_footer *allocatedFooterPtr = (sf_footer *)((char *) allocatedHeader + paddedSize + 8);
    char *freeHeaderPtr = (char*)allocatedFooterPtr+8;
    sf_footer *oldFreeFooter =(sf_footer *)(freeHeaderPtr + freeBlockSize - 8);


    sf_header *newFreeHeader = set_header_bits((sf_header *)freeHeaderPtr, false, false, freeBlockSize);
    set_footer_bits(oldFreeFooter, false, false, freeBlockSize, freeBlockSize);
    //sf_blockprint(newFreeHeader);
    return newFreeHeader;
}

void updateFreeList(sf_free_header *headNode, sf_header *freeHeader){
    //size_t size = freeHeader->block_size;


}

void removeFromList(sf_free_header *target){
    sf_header data = target->header;
    size_t blockSize = data.block_size << 4;
    int list_ind = getListIndex(blockSize);
    sf_free_header **listHead = &seg_free_list[list_ind].head;

    // If target is the headnode
    if(*listHead == target){
        *listHead = NULL;
    }

    // Change next only if node to be deleted is NOT the last node
    if(target->next != NULL){
        target->prev->next = target->next;
    }

    // Change prev only if node to be deleted is NOT the first node
    if(target->prev != NULL){
        target->prev->next = target->next;
    }
}

void appendToList(sf_header *node){
    size_t blockSize = (node)->block_size << 4;
    int list_ind = getListIndex(blockSize);
    sf_free_header *cand = (sf_free_header *) node;
    sf_free_header **listHead = &seg_free_list[list_ind].head;
    cand->prev = NULL;
    cand -> next = *listHead;

    if(*listHead != NULL){
        (*listHead)->prev = cand;
    }

    *listHead = cand;
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

int getListIndex(size_t size){
    if(size <LIST_1_MAX){
        return 0;
    }
    else if(size <LIST_2_MAX){
        return 1;
    }
    else if(size <LIST_3_MAX){
        return 2;
    }
    else {
        return 3;
    }
}