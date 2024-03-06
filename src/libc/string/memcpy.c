// string/memcpy.c // ~ eylon

#include <string.h>
#include <stdint.h>
#include <stddef.h>

/*  Copy memory area.

    The memcpy() function copies [n] bytes from memory area [src] to 
    memory area [dest]. The memory areas must not overlap. Use memmove 
    if the memory areas do overlap.
    
    The memcpy() function returns a pointer to [dest].

    #include <string.h>
    void* memcpy(void* dest, const void* src, size_t n);
*/
void* memcpy(void* dest, const void* src, size_t n)
{
    for(size_t i = 0; i < n; i++)
    {
        ((uint8_t*) dest)[i] = ((uint8_t*) src)[i];
    } 

    return dest;
}