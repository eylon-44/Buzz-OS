// errno/errno.c // ~ eylon

#include <errno.h>

/*  Global errno variable.

    The variable errno which is set by system calls and some library functions is used to
    provide detailed descriptions of system- or library-level error conditions. The value
    in errno is significant only when the return value of the call indicates an error (i.e.,
    -1 from most system calls; -1 or NULL from most library functions).

    #include <errno.h>
    int errno;
*/
int errno;