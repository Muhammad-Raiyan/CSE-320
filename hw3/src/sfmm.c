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
//static int page_count = 0;
int sf_errno = 0;
static int count_page = 0;
void *sf_malloc(size_t size) {
    void* memStart = NULL;

    if(size == 0 || size > 4*PAGE_SZ){
        sf_errno = EINVAL;
        return NULL;
    }

    if(count_page == 0){
        /* TODO CHECK HEAP OVERFLOW */
        memStart = sf_sbrk();

        count_page++;
        char *heapStart = (char *)get_heap_start();
        char *heapEnd = (char *) get_heap_end() - 8;

        debug("Heap Start %p\n", heapStart);
        debug("Head end %p\n", heapEnd);
        debug("%ld\n", heapStart - heapEnd);
        sf_free_header *free_header_node = (sf_free_header * ) set_header_bits((void *)heapStart, false, false, PAGE_SZ);
        free_header_node->next = NULL;
        free_header_node->prev = NULL;
        set_footer_bits((void *)heapEnd , false, false, PAGE_SZ, PAGE_SZ);
        sf_blockprint(memStart);
        seg_free_list[3].head = free_header_node;
    }

    sf_free_header *targetListHead = get_seg_free_list_header(seg_free_list, size);

    sf_free_header *targetNode = getFreeBlock(targetListHead, size);
    sf_header *freeBlockHeader = getFreeBlockHeader(targetListHead, size);
    //sf_footer *freeBlockFooter = getBlockFooter(freeBlockHeader);

    size_t oldSize = freeBlockHeader->block_size << 4;
    void *payload = allocate_payload(freeBlockHeader, size);
    sf_blockprint(freeBlockHeader);
    //sf_snapshot();
    sf_header *newFreeHeader = getNewFreeHeader(freeBlockHeader, oldSize, size);
    //sf_snapshot();
    sf_blockprint(newFreeHeader);

    targetNode = (sf_free_header *)newFreeHeader;
    seg_free_list[3].head = targetNode;
    printf("%p\n", targetNode);
    sf_snapshot();
	return payload;
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
