// Keyboard Driver // ~ eylon

#include <drivers/keyboard.h>
#include <drivers/ports.h>
#include <cpu/interrupts/isr.h>

#include <drivers/screen.h>
static void keyboard_handler()
{
    kprint("aloha", VGA_BG_BLUE | VGA_TXT_RED);
}

void init_keyboard()
{
    //port_outb()

    set_interrupt_handler(IRQ1, keyboard_handler);
}