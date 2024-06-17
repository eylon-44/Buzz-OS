// mm.h // ~ eylon

#if !defined(__LIBC_MM_H)
#define __LIBC_MM_H

/* Memory constants and utils. */

#define KB(x) ((x) * 0x400)
#define MB(x) ((x) * 0x100000)
#define GB(x) ((x) * 0x40000000)

// Default page size
#define MM_PAGE_SIZE KB(4)

#endif