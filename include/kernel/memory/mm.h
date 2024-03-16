// Memory Manager Header File // ~ eylon

#if !defined(MM_H)
#define MM_H

#include <libc/stdint.h>

/* Custom Types */

typedef uint32_t paddr_t;  // clarify the use of physical address
typedef uint32_t vaddr_t;  // clarify the use of virtual address


/* Function Declarations */

void init_mm();

#endif