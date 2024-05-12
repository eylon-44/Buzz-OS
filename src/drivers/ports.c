// Ports Utils // ~ eylon

#include <drivers/ports.h>

// Read a byte from a port
inline uint8_t port_inb(uint16_t port)
{
    // "d"  (port)   :: load EDX with [port]
    // "=a" (result) :: set [result] as EAX when finished
    uint8_t result;
    __asm__ volatile ("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

// Write a byte to a port
inline void port_outb(uint16_t port, uint8_t data)
{
    // "d" (port) :: load EDX with [port]
    // "a" (data) :: load EAX with [data]
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Read a word from a port
inline uint16_t port_inw(uint16_t port)
{
    uint16_t result;
    __asm__ volatile ("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

// Write a word to a port
inline void port_outw(uint16_t port, uint16_t data)
{
    __asm__ volatile ("out %0, %1" : : "a" (data), "d" (port));
}

// Input from port to [dest] (4 bytes per call)
inline void insd(uint16_t port, void* dest, uint32_t count)
{
    __asm__ volatile("cld; rep insl" :
        "=D" (dest), "=c" (count) :
        "d" (port), "0" (dest), "1" (count) :
        "memory", "cc");
}

// Output from [src] to port (4 bytes per call)
inline void outsd(uint16_t port, const void* src, uint32_t count)
{
    __asm__ volatile("cld; rep outsl" :
        "=S" (src), "=c" (count) :
        "d" (port), "0" (src), "1" (count) :
        "cc");
}

// Wait a very small amount of time (1 to 4 microseconds)
inline void io_wait(void)
{
    port_outb(0x80, 0);
}