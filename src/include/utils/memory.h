// Memory Utils Header File // ~ eylon

#if !defined(MEMORY_UTILS_H)
#define MEMORY_UTILS_H

#include <utils/type.h>

void memcpy(const void* source, void* dest, u16_t nbytes);
void memset(void* source, u8_t value, u16_t nbytes);

#endif