// stdio/fgets.c // ~ eylon

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Get a string from a stream.

    The fgets() function shall read bytes from stream [stream] into the array pointed to
    by [s] until [n-1] bytes are read, or '\n' is read and transferred to [s], or an end-of-file
    condition is encountered. A null byte shall be written immediately after the last byte read
    into the array. If the end-of-file condition is encountered before any bytes are read, the
    contents of the array pointed to by [s] shall not be changed.

    Upon successful completion, fgets() shall return [s]. If the stream is at end-of-file,
    the end-of-file indicator for the stream shall be set and fgets() shall return a null pointer.
    If a read error occurs, the error indicator for the stream shall be
    set, fgets() shall return NULL.

    #include <stdio.h>
    char* fgets(char* s, int n, int stream);
*/
char* fgets(char* s, int n, int stream)
{
    s[syscall(SYS_read, stream, s, n-1)] = '\0';
    return s;
}