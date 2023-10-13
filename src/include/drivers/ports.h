// Ports utils header file // ~ eylon

#if !defined(PORTS_H)
#define PORTS_H

#include "../utils/type.h"

// Byte I/O
u8_t port_inb(u16_t port);
void port_outb(u16_t port, u8_t data);

// Word I/O
u16_t port_inw(u16_t port);
void port_outw(u16_t port, u16_t data);

#endif