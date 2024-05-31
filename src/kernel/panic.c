// Kernel Panic Command // ~ eylon

#include <kernel/panic.h>
#include <drivers/screen.h>

// [TODO] imporove function

// Kernel panic routine
void KPANIC(char* msg)
{
    // disable interrupts
    __asm__ volatile ("cli");

    // clear the screen and print error message
    clear_screen();
    kprint("KERNEL PANIC", VGA_ATR_ERROR);
    kprint(" ---> ", VGA_ATR_DEFAULT);
    kprint(msg, VGA_ATR_WARNING);

    // halt forever
    for (;;) { __asm__ volatile ("hlt"); }
}