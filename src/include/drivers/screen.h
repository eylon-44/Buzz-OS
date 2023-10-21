// Screen driver header file // ~ eylon

#if !defined(SCREEN__DRIVER_H)
#define SCREEN_DRIVER_H

#include <utils/type.h>

// Screen measurements
#define SCREEN_MAX_ROWS 25
#define SCREEN_MAX_COLS 80
#define SCREEN_SIZE (SCREEN_MAX_ROWS * SCREEN_MAX_COLS)

// Utils
#define SCREEN_OFFSET(row, column) ((column) + (row) * SCREEN_MAX_COLS)

void kprint(char* string, u8_t attribute);
void kprint_at(char* string, u8_t attribute, u16_t offset);
void set_cursor_offset(u16_t offset);
u16_t get_cursor_offset();
void clear_screen();

// VGA attribute table //

// (0-2)                      |||
#define VGA_TXT_BLACK  0b00000000
#define VGA_TXT_BLUE   0b00000001
#define VGA_TXT_GREEN  0b00000010
#define VGA_TXT_CYAN   0b00000011
#define VGA_TXT_RED    0b00000100
#define VGA_TXT_PURPLE 0b00000101
#define VGA_TXT_ORANGE 0b00000110
#define VGA_TXT_WHITE  0b00000111
// (3)                       |
#define VGA_TXT_LIGHT  0b00001000   // make text color lighter
// (4-6)                  |||
#define VGA_BG_BLACK   0b00000000
#define VGA_BG_BLUE    0b00010000
#define VGA_BG_GREEN   0b00100000
#define VGA_BG_CYAN    0b00110000
#define VGA_BG_RED     0b01000000
#define VGA_BG_PINK    0b01010000
#define VGA_BG_ORANGE  0b01100000
#define VGA_BG_WHITE   0b01110000
// (7)                   |
#define VGA_BG_LIGHT   0b10000000   // make background color lighter


// VGA text attribute presets //

// [NOTE] it is prefered to use the presets over the custom combinations for a painless future :)
#define VGA_ATR_DEFAULT (VGA_TXT_WHITE | VGA_TXT_LIGHT | VGA_BG_BLUE)
#define VGA_ATR_ERROR   (VGA_TXT_BLACK | VGA_BG_RED)
#define VGA_ATR_WARNING (VGA_TXT_BLACK | VGA_BG_ORANGE | VGA_BG_LIGHT)

#endif