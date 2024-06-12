// Keyboard Driver Header File // ~ eylon

#if !defined(KEYBOARD_H)
#define KEYBOARD_H

// Modifier keys flag values
#define KB_MFLAG_SHIFT      (1<<0)      // shift is pressed
#define KB_MFLAG_CTRL       (1<<1)      // ctrl is pressed
#define KB_MFLAG_ALT        (1<<2)      // alt is pressed
#define KB_MFLAG_CAPSLOCK   (1<<3)      // caps lock is on
#define KB_MFLAG_NUMLOCK    (1<<4)      // numlock is on
#define KB_MFLAG_SCROLLLOCK (1<<5)      // scroll lock is on
#define KB_MFLAG_E0ESC      (1<<6)      // E0 escape character

void init_keyboard();

#endif