// Memory Utils Header File // ~ eylon

#if !defined(MEMORY_UTILS_H)
#define MEMORY_UTILS_H

#include <libc/stdint.h>

void memcpy(const void* source, void* dest, uint16_t nbytes);
void memset(void* source, uint8_t value, uint16_t nbytes);

#endif