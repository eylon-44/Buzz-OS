// stdlib.h // ~ eylon

#if !defined(__LIBC_STDLIB_H)
#define __LIBC_STDLIB_H

// Alignment utils
#define ALIGN_DOWN(value, align)   ((value) - ((value) % (align)))
#define ALIGN_UP(value, align)     ((value) + (((align) - ((value) % (align))) % (align))) 

#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
void exit(int status);
int atoi(const char *nptr);
void itoa(int n, char s[16]);
uint32_t rand();
void srand(unsigned int seed);

#endif