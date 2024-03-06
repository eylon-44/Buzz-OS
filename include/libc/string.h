// string.h // ~ eylon

#if !defined(__LIBC_STRING_H)
#define __LIBC_STRING_H

#include <stddef.h>

int memcmp(const void* s1, const void* s2, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int strcmp(const char* s1, const char* s2);
char* strcpy(char* dest, const char* src);
size_t strlen(const char* s);
int strncmp(const char* s1, const char* s2, size_t n);
char* strncpy(char* dest, const char* src, size_t n);

#endif