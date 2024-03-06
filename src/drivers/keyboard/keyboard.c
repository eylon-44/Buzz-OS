// Keyboard Driver // ~ eylon

#include <drivers/keyboard.h>
#include <drivers/ports.h>
#include <cpu/interrupts/isr.h>
#include <libc/stdint.h>
#include "keymap.h"

// IO ports
#define KB_SCAN_CODE_PORT   0x60     // keyboard scan_code port
#define KB_STAT_PORT   0x64     // keyboard controller status port

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
        modifiers |= MFLAG_E0ESC;
        return '\0';
    }
    // if key got released (key up) :: if it's a modifier key turn off its flag, turn off the E0 escape flag
    else if (scan_code & RELEASED_KEY_FLAG){
        // if last scan code was not an E0 escape, turn off the released-key flag from the scan code
        // this lets the key to be compared with the mflags_shift_map that includes only key_down scan codes
        if (!(modifiers & MFLAG_E0ESC)) scan_code &= ~RELEASED_KEY_FLAG;
        // if it's a shiftable modifier key turn off its flag, turn off the E0 escape flag
        modifiers &= ~(mflags_shift_map[scan_code] | MFLAG_E0ESC);
        return '\0';
    }
    // if last scan code was an E0 escape :: turn off the E0
    else if (modifiers & MFLAG_E0ESC){
        // turn off the release-key flag :: lets the key to be compared with the maps that include only key_down scan codes
        scan_code &= ~RELEASED_KEY_FLAG;
        // turn off the E0 escape flag
        modifiers &= ~MFLAG_E0ESC;
    }

    // update modifiers flag
    modifiers |= mflags_shift_map[scan_code];
    modifiers ^= mflags_toggle_map[scan_code];

    // calculate the map index to be used and get the ascii character from it
    // the map index is the first two bits of the modifier flag (control and shift flags)
    ascii = ascii_maps[modifiers & 0b00000011][scan_code];

    // if CapsLock flag is on :: uppercase/lowercase -> lowercase/uppercase
    if (modifiers & MFLAG_CAPSLOCK){
        if (ascii >= (uint8_t) 'a' && ascii <= (uint8_t) 'z')       { ascii &= ~ASCII_LOWERCASE_FLAG; }  // convert to uppercase
        else if (ascii >= (uint8_t) 'A' && ascii <= (uint8_t)  'Z') { ascii |= ASCII_LOWERCASE_FLAG; }   // convert to lowercase
    }

    return ascii;
}

// Keyboard interrupt handler :: handle keyboard callbacks from the PIC
#include <drivers/screen.h> // [TMP][DEBUG]
static void keyboard_handler()
{
    // [TMP]
    char key[2] = {get_key(), '\0'};
    kprint(key, VGA_ATR_DEFAULT);
    // [TODO] call a kernel function to handle the ascii input
}

// Initiate the keyboard driver
void init_keyboard() {
    // set the keyboard interrupt handler
    set_interrupt_handler(IRQ1, keyboard_handler);
}