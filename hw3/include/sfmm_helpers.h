
#include "sfmm.h"

#define BIT(n) ( 1<<(n) )
#define SET_BIT(val, bitIndex) val |= (1 << bitIndex)
#define CLEAR_BIT(val, bitIndex) val &= ~(1 << bitIndex)
#define TOGGLE_BIT(val, bitIndex) val ^= (1 << bitIndex)
#define BIT_IS_SET(val, bitIndex) (val & (1 << bitIndex))

bool needs_new_page();
void set_header_bits(sf_header *header, bool allocate, bool paddded, size_t blk_size);
void set_footer_bits(sf_footer *footer,  bool allocate, bool padded, size_t blk_size, size_t req_size);
