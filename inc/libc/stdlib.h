// stdlib.h // ~ eylon

#if !defined(__LIBC_STDLIB_H)
#define __LIBC_STDLIB_H

void exit(int status);
int atoi(const char *nptr);
void itoa(int n, char s[16]);

#endif