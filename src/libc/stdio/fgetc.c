// stdio/fgetc.c // ~ eylon

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Get a single character from a stream.

    fgetc() reads the next character from [stream] and returns it as a char cast to
    an int. If no bytes were read, the function returns '\0'. fgetc() may block if the input buffer is empty.

    #include <stdio.h>
    int fgetc(int stream);
*/
int fgetc(int stream)
{
    char chr;
    if (syscall(SYS_read, stream, &chr, 1) == 0) {
        chr = '\0';
    }
    return (int) chr;
}