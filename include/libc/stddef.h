// stddef.h // ~ eylon

#if !defined(__LIBC_STDDEF_H)
#define __LIBC_STDDEF_H

#include <stdint.h>

typedef uint32_t size_t;
typedef uint16_t wchar_t;

#define NULL ((void*)(0))
#define offsetof(type, member) ((size_t)&((type *)0)->member)
#define UNUSED __attribute__((unused))

#endif