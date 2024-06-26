// limits.h // ~ eylon

#if !defined(__LIBC_LIMITS_H)
#define __LIBC_LIMITS_H

#define ARGC_MAX        32
#define FNAME_MAX       64
#define PATH_MAX        256
#define PRIORITY_MIN    0
#define PRIORITY_MAX    100
#define SCREEN_ROWS_MAX 24
#define SCREEN_COLS_MAX 80

#define CHAR_BIT        8
#define SCHAR_MIN       -128
#define SCHAR_MAX       127
#define UCHAR_MAX       255
#define CHAR_MIN        -128
#define CHAR_MAX        127
#define SHRT_MIN        -32768
#define SHRT_MAX        32767
#define USHRT_MAX       65535
#define INT_MIN         -2147483648
#define INT_MAX         2147483647
#define UINT_MAX        4294967295

#endif