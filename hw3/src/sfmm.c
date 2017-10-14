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
        if(!canCoalesce(temp_free_header)) appendToList(temp_free_header);
        else {
            coalesce(temp_free_header);
        }
        tempTargetListHead = get_seg_free_list_head(seg_free_list, size);
    }

    sf_free_header *targetListHead = get_seg_free_list_head(seg_free_list, size);
    sf_free_header *targetNode = getFreeBlock(targetListHead, size);
    sf_header *freeBlockHeader = &(targetNode->header);
    //sf_snapshot();
    removeFromList(targetNode);
    //sf_snapshot();

    size_t oldSize = freeBlockHeader->block_size << 4;
    void *payload = allocate_payload(freeBlockHeader, size);
    sf_header *newFreeHeader = getNewFreeHeader(freeBlockHeader, oldSize, size);
    appendToList(newFreeHeader);

    //sf_snapshot();
    //sf_mem_fini();
	return payload;

    errorEINVAL:
        sf_errno = EINVAL;
        //sf_mem_fini();
        return NULL;

    errorENOMEM:
        sf_errno = ENOMEM;
        //sf_mem_fini();
        return NULL;
}

void *allocate_payload(sf_header *header, size_t size){
    bool needsPadding = false;
    size_t paddedSize = get_padded_size(size);
    if(size != paddedSize) needsPadding = true;
    size_t blockSize = paddedSize + 16;

    char *allocBlockHeader = set_header_bits(header, true, needsPadding, blockSize);
    char *payloadPtr = (char *)allocBlockHeader + 8;
    char *footerPtr = ((char *) allocBlockHeader + paddedSize +8);
    set_footer_bits((sf_footer *)footerPtr, true, needsPadding, blockSize, size+16);
    return (void *) payloadPtr;
}


void *sf_realloc(void *ptr, size_t size) {
	return NULL;
}

void sf_free(void *ptr) {
	return;
}

void coalesce(sf_header *currentHeader){
    sf_footer *prevFooter = (sf_footer *)((char *)currentHeader-8);

    size_t currentBlockSize = currentHeader->block_size << 4;
    size_t prevBlockSize = prevFooter->block_size << 4;

    sf_footer *currentFooter = (sf_footer *)((char *)currentHeader+currentBlockSize-8);
    sf_header *prevHeader = (sf_header *)((char *)prevFooter-prevBlockSize+8);
    removeFromList((sf_free_header *)prevHeader);
    size_t newBlockSize = prevBlockSize + currentBlockSize;
    prevHeader->block_size = newBlockSize >> 4;
    currentFooter->block_size = newBlockSize >> 4;
    appendToList(prevHeader);
}

