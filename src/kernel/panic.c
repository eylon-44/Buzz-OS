// Kernel Panic Command // ~ eylon

#include <kernel/panic.h>
#include <drivers/screen.h>
#include <kernel/ui.h>

// [TODO] imporove function

// Kernel panic routine
void KPANIC(char* msg)
{
    // disable interrupts
    __asm__ volatile ("cli");

    // clear the screen and print error message
    vga_clear();
    vga_print("KERNEL PANIC", UI_ATR_ERROR);
    vga_print(" ---> ", UI_ATR_DEFAULT);
    vga_print(msg, UI_ATR_WARNING);

    // halt forever
    for (;;) { __asm__ volatile ("hlt"); }
}