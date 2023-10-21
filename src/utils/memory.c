// Memory Utils // ~ eylon

#include <utils/memory.h>
#include <utils/type.h>

// Memory copy :: copy [nbytes] from [source] to [dest] pointer
void memcpy(const void* source, void* dest, u16_t nbytes)
{
    for (u16_t i = 0; i < nbytes; i++) {
        ((u8_t*) dest)[i] = ((u8_t*) source)[i];
    }
}

// Memory set :: set [nbytes] of [source] pointer to [value]
void memset(void* source, u8_t value, u16_t nbytes)
{
    for (u16_t i = 0; i < nbytes; i++) {
        ((u8_t*) source)[i] = value;
    }
}