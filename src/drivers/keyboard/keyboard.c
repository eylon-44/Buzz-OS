// Keyboard Driver // ~ eylon

#include <drivers/keyboard.h>
#include "keymap.h"
#include <drivers/ports.h>
#include <kernel/interrupts/isr.h>
#include <kernel/ui.h>
#include <libc/stdint.h>
#include <libc/stddef.h>

// IO ports
#define KB_SCAN_CODE_PORT   0x60     // keyboard scan_code port
#define KB_STAT_PORT   0x64     // keyboard controller status port

// Array of void functions(char key, uint8_t modifiers) to be called uppon a keyboard interrupt 
static void (*callbacks[])(char key, uint8_t modifiers) = { ui_key_event_handler };

static uint8_t modifiers = 0; // modifier keys flags

// Read keyboard input and convert the scan code into an ascii
static char get_key()
{
    uint8_t status, scan_code, ascii;

    // check status register to see if the keyboard buffer has a scan_code to read
    status = port_inb(KB_STAT_PORT);
    if (!(status & KB_BUFFER_FULL)) {
        return '\0';  // buffer is empty, nothing to read
    }
    // read the scan code from the keyboard's scan_code port
    scan_code = port_inb(KB_SCAN_CODE_PORT);


    // if it's an E0 escape :: set the E0 escape modifier flag and return a null character
    if (scan_code == E0_ESC) {
        modifiers |= KB_MFLAG_E0ESC;
        return '\0';
    }
    // if key got released (key up) :: if it's a modifier key turn off its flag, turn off the E0 escape flag
    else if (scan_code & RELEASED_KEY_FLAG){
        // if last scan code was not an E0 escape, turn off the released-key flag from the scan code
        // this lets the key to be compared with the mflags_shift_map that includes only key_down scan codes
        if (!(modifiers & KB_MFLAG_E0ESC)) scan_code &= ~RELEASED_KEY_FLAG;
        // if it's a shiftable modifier key turn off its flag, turn off the E0 escape flag
        modifiers &= ~(mflags_shift_map[scan_code] | KB_MFLAG_E0ESC);
        return '\0';
    }
    // if last scan code was an E0 escape :: turn off the E0
    else if (modifiers & KB_MFLAG_E0ESC){
        // turn off the release-key flag :: lets the key to be compared with the maps that include only key_down scan codes
        scan_code &= ~RELEASED_KEY_FLAG;
        // turn off the E0 escape flag
        modifiers &= ~KB_MFLAG_E0ESC;
    }

    // update modifiers flag
    modifiers |= mflags_shift_map[scan_code];
    modifiers ^= mflags_toggle_map[scan_code];

    // calculate the map index to be used and get the ascii character from it
    // the map index is the first two bits of the modifier flag (control and shift flags)
    ascii = ascii_maps[modifiers & 0b00000001][scan_code];

    // if CapsLock flag is on :: uppercase/lowercase -> lowercase/uppercase
    if (modifiers & KB_MFLAG_CAPSLOCK){
        if (ascii >= (uint8_t) 'a' && ascii <= (uint8_t) 'z')       { ascii &= ~ASCII_LOWERCASE_FLAG; }  // convert to uppercase
        else if (ascii >= (uint8_t) 'A' && ascii <= (uint8_t)  'Z') { ascii |= ASCII_LOWERCASE_FLAG; }   // convert to lowercase
    }

    return ascii;
}

/* This function is being called uppon a keyboard interrupt.
    The function calls all of the functions in the [callbacks] array with
    the pressed key and the modifiers. */
static void keyboard_handler(UNUSED int_frame_t*)
{
    char key = get_key();
    if (key != '\0') {
        for (size_t i = 0; i < sizeof(callbacks)/sizeof(callbacks[0]); i++) {
            callbacks[i](key, modifiers);
        }
    }

}

// Initiate the keyboard driver
void init_keyboard() {
    // set the keyboard interrupt handler
    set_interrupt_handler(IRQ1, keyboard_handler);
}