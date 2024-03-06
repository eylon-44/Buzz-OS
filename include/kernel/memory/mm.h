// Memory Manager Header File // ~ eylon

#if !defined(MM_H)
#define MM_H

#include <libc/stdint.h>
//#include "paging.h"

// [TODO] BITMAP???? PHYSICAL!! move it to there if no one else uses it
// Get the page number from its index in the array
#define INDX_TO_PAGE_NUM(element, index) ((uint32_t) (element)*32+(index))
// Get the page number from its physical base address
#define BASE_TO_PAGE_NUM(base_address)   ((uint32_t) (base_address)/MM_PAGE_SIZE)

#define KB(x) (uint32_t) ((x) * 0x400)
#define MB(x) (uint32_t) ((x) * 0x100000)
#define GB(x) (uint32_t) ((x) * 0x40000000)

typedef uint32_t paddr_t;  // clarify the use of physical address
typedef uint32_t vaddr_t;  // clarify the use of virtual address


void init_mm();

#endif