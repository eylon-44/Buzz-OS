// Keyboard Driver Header File // ~ eylon

#if !defined(KEYBOARD_H)
#define KEYBOARD_H

// IO ports
#define PS2_DATA_PORT    0x60             // data port (read/write)
#define PS2_STATUS_PORT  0x64             // status register (read)
#define PS2_CMD_PORT     0x64             // command port (write)

// Commands
#define KB_CMD_LED        0xED          // set scroll-lock/number-lock/caps-lock leds
#define KB_CMD_LED_SCROLL 0b001         // scroll-lock led on 
#define KB_CMD_LED_NUMBER 0b010         // number-lock led on
#define KB_CMD_LED_CAPS   0b100         // caps-lock led on

#define KB_CMD_SCAN_CODE_SET 0xF0       // set scan code set, sub-command: 
                                        // 0x0     : get current scan code set
                                        // 0x1-0x3 : set scan code set number [X]

#define KB_CMD_SET_TYPEMATIC 0xF3       // set typematic rate and delay, data:
                                        // (0-4) repeat rate : 00000b = 30 Hz, ..., 11111b = 2 Hz
                                        // (5-6) delay before key repeat : 00b = 250 ms, 01b = 500 ms, 10b = 750 ms, 11b = 1000 ms
                                        // (7)   reserved 0

#define KB_CMD_ENABLE_SCANNING  0xF4    // enable scanning, keyboard will send scan codes
#define KB_CMD_DISABLE_SCANNING 0xF5    // disable scanning, keyboard will not send scan codes

#define KB_CMD_SET_DEFAULT 0xF6         // set default parameters
#define KB_CMD_RESEND_LAST_BYTE 0xFE    // resend last byte
#define KB_CMD_REST 0xFF                // reset keyboard

void init_keyboard();

#endif