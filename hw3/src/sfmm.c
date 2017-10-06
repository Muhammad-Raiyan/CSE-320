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

void *sf_malloc(size_t size) {
    void* mem_start = NULL;

    if(size == 0 || size > 4*PAGE_SZ){
        sf_errno = EINVAL;
        return NULL;
    }


    if(get_heap_start()==NULL){
        /* TODO CHECK HEAP OVERFLOW */
        mem_start = sf_sbrk();
        sf_header *allocated_mem = mem_start;
        //sf_footer *alloc_boundary = (void *)allocated_mem+size;

        set_header_bits(allocated_mem, true, false, size);
        //set_footer_bits(alloc_boundary, true, false, size, size);
        printf("%p\n", allocated_mem);
        //printf("%p\n", alloc_boundary);
    }
	return mem_start;
}



void *sf_realloc(void *ptr, size_t size) {
	return NULL;
}

void sf_free(void *ptr) {
	return;
}
