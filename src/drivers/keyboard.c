// Keyboard Driver // ~ eylon

#include <drivers/keyboard.h>
#include <drivers/ports.h>
#include <cpu/interrupts/isr.h>
#include <utils/type.h>

#include <drivers/screen.h>
static void keyboard_handler()
{
    u8_t scancode = port_inb(0x60);
    UNUSED(scancode);
    kprint("KEY", VGA_BG_BLUE | VGA_TXT_RED);
}

void init_keyboard() {
    set_interrupt_handler(IRQ1, keyboard_handler);
}