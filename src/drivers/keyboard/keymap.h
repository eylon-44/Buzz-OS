// Key Maps Header File Of Scan Code Set 1 :: Keyboard Driver Util // ~ eylon

#if !defined(KEYMAP_H)
#define KEYMAP_H

// Keyboard status register's bit 0 :: 0=buffer empty, 1=buffer full
#define KB_BUFFER_FULL 0b00000001

// Flags & Prefixes
#define E0_ESC               0xE0           // E0 escape scan code :: function keys prefix code (home, arrow keys, etc...)
#define RELEASED_KEY_FLAG    0b10000000     // released-key flag :: when this bit is set in the scan code, the key was released
#define ASCII_LOWERCASE_FLAG 0b00100000     // lowercase ascii flag :: 0=UPPERCASE, 1=LOWERCASE

// Special keycodes
#define KEY_HOME 0xE0
#define KEY_END  0xE1
#define KEY_UP   0xE2
#define KEY_DN   0xE3
#define KEY_LF   0xE4
#define KEY_RT   0xE5
#define KEY_PGUP 0xE6
#define KEY_PGDN 0xE7
#define KEY_INS  0xE8
#define KEY_DEL  0xE9
#define KEY_ESC  0x1B


// Scan Code to shiftable Modifier Flag map
static const unsigned char mflags_shift_map[256] =
{
    [0x1D] KB_MFLAG_CTRL,
    [0x9D] KB_MFLAG_CTRL,
    [0x2A] KB_MFLAG_SHIFT,
    [0x36] KB_MFLAG_SHIFT,
    [0x38] KB_MFLAG_ALT,
    [0xB8] KB_MFLAG_ALT
};

// Scan Code to toggleable Modifier Flag map
static const unsigned char mflags_toggle_map[256] =
{
    [0x3A] KB_MFLAG_CAPSLOCK,
    [0x45] KB_MFLAG_NUMLOCK,
    [0x46] KB_MFLAG_SCROLLLOCK
};


/* Scan Code to Ascii maps */

#define NO 0x00               // placeholder for undefined keys in the ascii map

static const unsigned char default_map[256] =
{
    NO, KEY_ESC, '1', '2', '3', '4', '5', '6',  // 0x00
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',     // 0x10
    'o', 'p', '[', ']', '\n', NO, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     // 0x20
    '\'' ,'`', NO, '\\','z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', NO, '*',      // 0x30
    NO, ' ', NO, NO, NO, NO, NO, NO,
    NO, NO, NO, NO, NO, NO, NO, '7',            // 0x40
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', NO, NO, NO, NO,         // 0x50

    [0x9C] '\n',      // keypad enter
    [0xB5] '/',       // keypad divide
    [0xC8] KEY_UP,    [0xD0] KEY_DN,
    [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
    [0xCB] KEY_LF,    [0xCD] KEY_RT,
    [0x97] KEY_HOME,  [0xCF] KEY_END,
    [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static const unsigned char shift_map[256] =
{
    NO, KEY_ESC, '!', '@', '#', '$', '%', '^',  // 0x00
    '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',     // 0x10
    'O', 'P', '{', '}', '\n', NO,  'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     // 0x20
    '"', '~', NO,  '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', NO,  '*',     // 0x30
    NO,  ' ', NO,  NO,  NO,  NO,  NO,  NO,
    NO,  NO,  NO,  NO,  NO,  NO,  NO,  '7',     // 0x40
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', NO,  NO, NO, NO,        // 0x50

    [0x9C] '\n',      // keypad enter
    [0xB5] '/',       // keypad divide
    [0xC8] KEY_UP,    [0xD0] KEY_DN,
    [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
    [0xCB] KEY_LF,    [0xCD] KEY_RT,
    [0x97] KEY_HOME,  [0xCF] KEY_END,
    [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static const unsigned char* ascii_maps[2] = {
    default_map, shift_map
};

#endif