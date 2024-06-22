// string/strlcpy.c // ~ eylon

#include <string.h>
#include <stddef.h>

/* Size bounded string copying.
       
    The strlcpy() function supplies a safe way for copying strings between buffers. strlcpy()
    takes the full size of the buffer (not just the length) and guarantees to null-terminate the
    result (as long as there is at least one byte free in [dst]). Note that a byte for the null-terminator
    should be included in [size] and in [src].

    The function copies up to size [size]-1 characters from the null-terminated string [src] to
    [dst], and null-terminates the result.

    The function returns the length of [src].

    #include <string.h>
    char* strlcpy(char* dest, const char* src, size_t size);
*/
size_t strlcpy(char* dest, const char* src, size_t size)
{
    const size_t srclen = strlen(src);
    
    if (srclen + 1 < size) {
        memcpy(dest, src, srclen + 1);
    }
    else if (size != 0) {
        memcpy(dest, src, size - 1);
        dest[size-1] = '\0';
    }
    
    return srclen;
}