// Kernel Panic Command // ~ eylon

#include <kernel/panic.h>
#include <drivers/screen.h>

// [TODO] imporove function

// Kernel panic routine
void panic(char* msg)
{
    // disable interrupts
    __asm__ volatile ("cli");

    // clear the screen and print error message
    clear_screen();
    kprint(msg, VGA_ATR_ERROR);

    // halt forever
    for (;;) { __asm__ volatile ("hlt"); }
}