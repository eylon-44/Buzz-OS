// ctype.h // ~ eylon

#if !defined(__LIBC_CTYPE_H)
#define __LIBC_CTYPE_H

int tolower(int c);
int toupper(int c);
int isalnum(int c);
int isalpha(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

#endif