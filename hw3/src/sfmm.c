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
    void* mem_start = NULL;

    if(size == 0 || size > 4*PAGE_SZ){
        sf_errno = EINVAL;
        return NULL;
    }

    if(count_page == 0){
        /* TODO CHECK HEAP OVERFLOW */
        mem_start = sf_sbrk();
        count_page++;
        char *heap_start = (char *)get_heap_start();
        char *heap_end = (char *) get_heap_end() - 8;
        /*printf("%p\n", heap_start);
        printf("%p\n", heap_end);
        printf("%ld\n", heap_start - heap_end);*/
        printf("Heap Start %p\n", heap_start);
        sf_free_header *free_header_node = (sf_free_header * ) set_header_bits((void *)heap_start, false, false, PAGE_SZ);
        free_header_node->next = NULL;
        free_header_node->prev = NULL;
        set_footer_bits((void *)heap_end , false, false, PAGE_SZ, PAGE_SZ);

        seg_free_list[3].head = free_header_node;
    }

    sf_free_header *allocate_from_list =get_seg_free_list_header(seg_free_list, size);
    printf("List: %p\n", allocate_from_list);
    //sf_header *allocHead = getFreeBlock(allocate_from_list);
	return mem_start;
}





void *sf_realloc(void *ptr, size_t size) {
	return NULL;
}

void sf_free(void *ptr) {
	return;
}
