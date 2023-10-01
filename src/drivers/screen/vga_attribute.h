// VGA attribute table // ~ eylon

#if !defined(VGA_ATRB_H)
#define VGA_ATRB_H

// (0-2)                      |||
#define VGA_TXT_BLACK  0b00000000
#define VGA_TXT_BLUE   0b00000001
#define VGA_TXT_GREEN  0b00000010
#define VGA_TXT_CYAN   0b00000011
#define VGA_TXT_RED    0b00000100
#define VGA_TXT_PURPLE 0b00000101
#define VGA_TXT_YELLOW 0b00000110
#define VGA_TXT_WHITE  0b00000111
// (3)                       |
#define VGA_TXT_BOLD   0b00001000
// (4-5)                   ||
#define VGA_BG_BLACK   0b00000000
#define VGA_BG_BLUE    0b00010000
#define VGA_BG_GREEN   0b00100000
#define VGA_BG_CYAN    0b00110000
// (6)                    |
#define VGA_BG_BOLD    0b01000000
// (7)                   |
#define VGA_BLINK      0b10000000


#endif

/*
-bits-     -attribute-
(0-2) : Foreground Color
(3)   : Foreground Intensity
(4-5) : Background Color
(6)   : Background Intensity
(7)   : Blinking
*/