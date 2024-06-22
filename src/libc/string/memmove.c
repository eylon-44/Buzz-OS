// string/memmove.c // ~ eylon

#include <string.h>
#include <stdint.h>
#include <stddef.h>

/*  Copy area memory.

    The memmove() function copies [n] bytes from memory area [src] to memory area [dest].
    The memory areas may overlap: copying takes place as though the bytes in [src] are first
    copied into a temporary array that does not overlap [src] or [dest], and the bytes
    are then copied from that temporary array to [dest].

    The function returns a pointer to [dest].

    #include <string.h>
    void* memmove(void* dest, const void* src, size_t n);
*/
void* memmove(void* dest, const void* src, size_t n)
{
	uint8_t* to   = (uint8_t*) dest;
	uint8_t* from = (uint8_t*) src;

	if (from == to || n == 0)
		return dest;


	if (to > from && to-from < (int) n) {
		/* [to] overlaps with [from]
		    <from......>
		           <to........>
	        copy in reverse, to avoid overwriting [from] */
		for(int i = n-1; i >= 0; i--)
			to[i] = from[i];
		return dest;
	}
	if (from > to && from-to < (int) n) {
		/* [to] overlaps with [from]
		          <from......>
		    <to........>
		    copy forwards, to avoid overwriting [from] */
		for(size_t i = 0; i < n; i++)
			to[i] = from[i];
		return dest;
	}
	memcpy(dest, src, n);

	return dest;
}