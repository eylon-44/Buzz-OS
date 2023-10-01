#if !defined(PORTS_H)
#define PORTS_H

#include "../utils/type.h"

// Byte I/O
u8_t port_byte_in(u16_t port);
void port_byte_out(u16_t port, u8_t data);

// Word I/O
u16_t port_word_in(u16_t port);
void port_word_out(u16_t port, u16_t data);

#endif