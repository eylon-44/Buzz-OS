// Ports Utils Header File // ~ eylon

#if !defined(PORTS_H)
#define PORTS_H

#include <utils/type.h>

u8_t port_inb(u16_t port);
void port_outb(u16_t port, u8_t data);
u16_t port_inw(u16_t port);
void port_outw(u16_t port, u16_t data);
void insd(u16_t port, void* dest, u32_t count);
void io_wait(void);

#endif