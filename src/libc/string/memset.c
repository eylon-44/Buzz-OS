// string/memset.c // ~ eylon

#include <string.h>
#include <stdint.h>

/*  Fill memory with a constant byte.

    The memset() function fills the first [n] bytes of the memory area 
    pointed to by [s] with the constant byte [c].
    
    The memset() function returns a pointer to the memory area [s].

    #include <string.h>
    void* memset(void* s, int c, size_t n);
*/
void* memset(void* s, int c, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((uint8_t*) s)[i] = c;
    }

    return s;
}