// string/strlcat.c // ~ eylon

#include <string.h>
#include <stddef.h>

/* Size bounded string concatenation.
       
    The strlcat() supplies a safe way for concatenating strings. strlcat() takes the full size of the
    buffer (not just the length) and guarantees to null-terminate the result (as long as size is larger than 0).
    Note that a byte for the null-terminator should be included in [size], [src] and [dest].

    The function appends the null-terminated string [src] to the end of [dest]. It will append at most
    [size]-strlen(dest)-1 bytes, while null-terminating the result.

    The function returns the initial length of [dest] plus the length of [src].

    #include <string.h>
    size_t strlcat(char* dest, const char* src, size_t size);
*/
size_t strlcat(char* dest, const char* src, size_t size)
{
    const size_t srclen = strlen(src);
    const size_t dstlen = strnlen(dest, size);

    if (dstlen == size) return size+srclen;
    if (srclen < size-dstlen) {
        memcpy(dest+dstlen, src, srclen+1);
    }
    else {
        memcpy(dest+dstlen, src, size-1);
        dest[dstlen+size-1] = '\0';
    }

    return dstlen + srclen;
}