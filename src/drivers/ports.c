// Ports Utils // ~ eylon

#include <drivers/ports.h>
#include <utils/type.h>

// Read a byte from a port
u8_t port_byte_in(u16_t port)
{
    // "d"  (port)   :: load EDX with [port]
    // "=a" (result) :: set [result] as EAX when finished
    u8_t result;
    __asm__ volatile ("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

// Write a byte to a port
void port_byte_out(u16_t port, u8_t data)
{
    // "d" (port) :: load EDX with [port]
    // "a" (data) :: load EAX with [data]
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Read a word from a port
u16_t port_word_in(u16_t port)
{
    u16_t result;
    __asm__ volatile ("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

// Write a word to a port
void port_word_out(u16_t port, u16_t data)
{
    __asm__ volatile ("out %0, %1" : : "a" (data), "d" (port));
}