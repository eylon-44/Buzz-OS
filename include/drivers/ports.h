// Ports Utils Header File // ~ eylon

#if !defined(PORTS_H)
#define PORTS_H

#include <libc/stdint.h>

uint8_t port_inb(uint16_t port);
void port_outb(uint16_t port, uint8_t data);
uint16_t port_inw(uint16_t port);
void port_outw(uint16_t port, uint16_t data);
void insd(uint16_t port, void* dest, uint32_t count);
void outsd(uint16_t port, const void* src, uint32_t count);
void io_wait(void);

#endif