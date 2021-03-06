/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sfmm_helpers.h"

/**
 * You should store the heads of your free lists in these variables.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
free_list seg_free_list[4] = {
    {NULL, LIST_1_MIN, LIST_1_MAX},
    {NULL, LIST_2_MIN, LIST_2_MAX},
    {NULL, LIST_3_MIN, LIST_3_MAX},
    {NULL, LIST_4_MIN, LIST_4_MAX}
};
int sf_errno = 0;
static int count_page = 0;

void *sf_malloc(size_t size) {
    //sf_mem_init();
    if(size <= 0 || size > 4*PAGE_SZ){
        goto errorEINVAL;
    }

    if(count_page >= 4){
        goto errorENOMEM;
    }

    if(count_page == 0){
        void* memStart = sf_sbrk();
        count_page++;
        appendToList(pageToMemoryBlock(memStart));
    }

    sf_free_header *tempTargetListHead = get_seg_free_list_head(seg_free_list, size);

    while(tempTargetListHead ==NULL){
        if(count_page >= 4){
            goto errorENOMEM;
        }
        void * pgStart= sf_sbrk();
        count_page++;
        sf_header *temp_free_header = pageToMemoryBlock(pgStart);
        if(!canCoalesceBack(temp_free_header)) appendToList(temp_free_header);
        else {
            coalesceBack(temp_free_header);
        }
        tempTargetListHead = get_seg_free_list_head(seg_free_list, size);
    }

    sf_free_header *targetListHead = get_seg_free_list_head(seg_free_list, size);
    sf_free_header *targetNode = getFreeBlock(targetListHead, size);
    sf_header *freeBlockHeader = &(targetNode->header);

    size_t oldSize = freeBlockHeader->block_size << 4;
    size_t allocSize = get_padded_size(size)+16;
    size_t sizeDiff = oldSize - allocSize;
    if(sizeDiff < 32) {
        size = size + sizeDiff;
        targetListHead = get_seg_free_list_head(seg_free_list, size);
        targetNode = getFreeBlock(targetListHead, size);
        freeBlockHeader = &(targetNode->header);
    }

    removeFromList(targetNode);
    void *payload = allocate_payload(freeBlockHeader, size);
    sf_header *newFreeHeader = getNewFreeHeader(freeBlockHeader, oldSize, size);
    if(newFreeHeader->block_size<<4!=0)appendToList(newFreeHeader);

	return payload;

    errorEINVAL:
        sf_errno = EINVAL;
        return NULL;

    errorENOMEM:
        sf_errno = ENOMEM;
        return NULL;
}

void *allocate_payload(sf_header *header, size_t size){
    bool needsPadding = false;
    size_t paddedSize = get_padded_size(size);
    if(size != paddedSize) needsPadding = true;
    size_t blockSize = paddedSize + SF_HEADER_SIZE/8 + SF_FOOTER_SIZE/8;

    char *allocBlockHeader = set_header_bits(header, true, needsPadding, blockSize);
    char *payloadPtr = (char *)allocBlockHeader + SF_HEADER_SIZE/8;
    char *footerPtr = ((char *) allocBlockHeader + paddedSize + SF_HEADER_SIZE/8);
    set_footer_bits((sf_footer *)footerPtr, true, needsPadding, blockSize, size);
    return (void *) payloadPtr;
}


void *sf_realloc(void *ptr, size_t size) {
    if(!isValidPtr(ptr)){
        goto error;
    }
    if(size == 0){
        sf_free(ptr);
        return NULL;
    }
    sf_header *givenHeader = (sf_header *)(ptr - SF_HEADER_SIZE/8);
    size_t givenBlockSize = givenHeader->block_size << 4;
    if(size > givenBlockSize)
        return sf_realloc_larger(ptr, size);
    if(size < givenBlockSize)
        return sf_realloc_smaller(ptr, size);
	return NULL;

    error:
        abort();
}

void *sf_realloc_larger(void *ptr, size_t size){
    void *newBlock = sf_malloc(size);
    newBlock = memcpy(newBlock, ptr, size);
    sf_free(ptr);
    return newBlock;
}

void *sf_realloc_smaller(void *ptr, size_t size){
    sf_header *givenHeader = (sf_header *)(ptr - SF_HEADER_SIZE/8);
    size_t givenBlockSize = givenHeader->block_size << 4;
    sf_footer *givenFooter = (sf_footer *)((char *)givenHeader+givenBlockSize-SF_FOOTER_SIZE/8);

    size_t newPldSize = get_padded_size(size);
    size_t newBlockSize = newPldSize + 16;
    if(splittingCreatesSplinter(givenHeader, size)){
        givenFooter->requested_size = size;
        if(get_padded_size(size) < givenBlockSize){
            givenHeader->padded = 1;
            givenFooter->padded = 1;
        }
        return ptr;
    } else {
        givenHeader->block_size = (newBlockSize>>4);
        sf_footer *newFooter = (sf_footer *)((char *)givenHeader+newBlockSize-SF_FOOTER_SIZE/8);
        set_footer_bits(newFooter, true, givenHeader->padded, newBlockSize, size);
        sf_header *newHeader = (sf_header *)((char *)newFooter+SF_FOOTER_SIZE/8);
        size_t newFreeBlockSize = givenBlockSize - newBlockSize;
        newHeader = set_header_bits(newHeader, false, false, newFreeBlockSize);
        set_footer_bits(givenFooter, false, false, newFreeBlockSize, 0);

        sf_header *nextNextHeader = (sf_header *)((char *) newHeader+newFreeBlockSize);
        appendToList(newHeader);
        //sf_snapshot();
        if (canCoalesceBack(nextNextHeader)){
            coalesceBack(nextNextHeader);
        }
    }
    return ((char *)givenHeader+SF_HEADER_SIZE/8);
}

void sf_free(void *ptr) {

    if(ptr == NULL)
        goto error;
    sf_header *givenHeader = (sf_header *)(ptr - SF_HEADER_SIZE/8);
    size_t givenBlockSize = givenHeader->block_size << 4;
    sf_footer *givenFooter = (sf_footer *)(ptr+givenBlockSize-SF_HEADER_SIZE/8-SF_FOOTER_SIZE/8);

    if(!isValidPtr(ptr))
        goto error;

    sf_header *nextHeader = (sf_header *)((char*)givenFooter + SF_FOOTER_SIZE/8);
    size_t nextBlockSize = nextHeader->block_size<<4;
    sf_footer *nextFooter = (sf_footer *)((char *)nextHeader+nextBlockSize-SF_HEADER_SIZE/8);

    sf_footer *targetFooter = givenFooter;
    size_t targetSize = givenBlockSize;
    if(nextHeader->allocated == 0){
        targetFooter = nextFooter;
        targetSize = givenBlockSize + nextBlockSize;
        removeFromList((sf_free_header *)nextHeader);
    }

    givenHeader = set_header_bits(givenHeader, false, false, targetSize);
    set_footer_bits(targetFooter, false, false, targetSize, 0);
    appendToList(givenHeader);
	return;

    error:
        abort();
}

bool isValidPtr(void *ptr){
    if(ptr == NULL)
        return false;
    sf_header *givenHeader = (sf_header *)(ptr - SF_HEADER_SIZE/8);
    size_t givenBlockSize = givenHeader->block_size << 4;
    sf_footer *givenFooter = (sf_footer *)(ptr+givenBlockSize-16);
    size_t givenBlockReqSize = givenFooter->requested_size;

    //if(givenHeader->allocated == 0) return false;

    if((void *)givenHeader-get_heap_start() < 0 || get_heap_end()-(void *) givenFooter < 0)
        return false;

    if(givenBlockReqSize+16!=givenBlockSize && givenHeader->padded ==0)
        return false;

    if(givenBlockReqSize+16==givenBlockSize && givenHeader->padded ==1)
        return false;
    if(givenHeader->allocated != givenFooter->allocated \
        || givenHeader->padded !=givenFooter->padded    \
        || givenHeader->block_size != givenFooter->block_size)
        return false;
    return true;
}

void coalesceBack(sf_header *currentHeader){
    sf_footer *prevFooter = (sf_footer *)((char *)currentHeader-SF_FOOTER_SIZE/8);

    size_t currentBlockSize = currentHeader->block_size << 4;
    size_t prevBlockSize = prevFooter->block_size << 4;

    sf_footer *currentFooter = (sf_footer *)((char *)currentHeader+currentBlockSize-SF_HEADER_SIZE/8);
    sf_header *prevHeader = (sf_header *)((char *)prevFooter-prevBlockSize+SF_FOOTER_SIZE/8);
    removeFromList((sf_free_header *)prevHeader);
    removeFromList((sf_free_header *)currentHeader);
    //sf_snapshot();
    size_t newBlockSize = prevBlockSize + currentBlockSize;
    prevHeader->block_size = newBlockSize >> 4;
    currentFooter->block_size = newBlockSize >> 4;
    appendToList(prevHeader);
}
