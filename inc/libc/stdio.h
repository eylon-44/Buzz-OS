// stdio.h // ~ eylon

#if !defined(__LIBC_STDIO_H)
#define __LIBC_STDIO_H

// LIBC file descriptor struct
typedef struct {
    int _fileno;
} FILE;

int printf(const char* format, ...);

#endif