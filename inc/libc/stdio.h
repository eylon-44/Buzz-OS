// stdio.h // ~ eylon

#if !defined(__LIBC_STDIO_H)
#define __LIBC_STDIO_H

#include <stdarg.h>

// LIBC file descriptor struct
typedef struct {
    int _fileno;
} FILE;

/* Streams */
#define stdin  0     // standard input
#define stdout 1     // standard output
#define stderr 2     // standard error

int printf(const char* format, ...);
int asprintf(char **strp, const char *fmt, ...);
int vasprintf(char **strp, const char *fmt, va_list ap);
char* fgets(char* s, int n, int stream);
int rename(const char* oldpath, const char* newpath);
int fgetc(int stream);

#endif