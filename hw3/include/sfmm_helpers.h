
#include "sfmm.h"

#define BIT(n) ( 1<<(n) )
#define SET_BIT(val, bitIndex) val |= (1 << bitIndex)
#define CLEAR_BIT(val, bitIndex) val &= ~(1 << bitIndex)
#define TOGGLE_BIT(val, bitIndex) val ^= (1 << bitIndex)
#define BIT_IS_SET(val, bitIndex) (val & (1 << bitIndex))

void* set_header_bits(sf_header *header, bool alloc, bool padded, size_t blk_size);
void set_footer_bits(sf_footer *footer,  bool alloc, bool padded, size_t blk_size, size_t req_size);
sf_header *pageToMemoryBlock(void *memStart);

size_t get_padded_size(size_t size);
sf_free_header *get_seg_free_list_head(free_list *list, size_t reqSize);
bool hasFreeBlock(sf_free_header *list, size_t reqSize);
sf_header *getFreeBlockHeader(sf_free_header *head, size_t size);
sf_footer *getBlockFooter(sf_header *header);
sf_free_header *getFreeBlock(sf_free_header *headNode, size_t size);

void *allocate_payload(sf_header *header, size_t size);
sf_header *getNewFreeHeader(sf_header *allocatedheader, size_t oldSize, size_t reqSize);
void removeFromList(sf_free_header *target);
int getListIndex(size_t size);
void appendToList(sf_header *node);

bool canCoalesce(sf_header *header);
void coalesce(sf_header *header);