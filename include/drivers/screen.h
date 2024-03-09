// Screen driver header file // ~ eylon

#if !defined(SCREEN__DRIVER_H)
#define SCREEN_DRIVER_H

#include <libc/stdint.h>

// Screen measurements
#define VGA_MAX_ROWS 25
#define VGA_MAX_COLS 80
#define VGA_SIZE (VGA_MAX_ROWS * VGA_MAX_COLS)

// Video memory
#define VGA_VIRT_MEM 0xC00B8000
#define VGA_PHYS_MEM 0x000B8000
#define VGA_MEM_SIZE VGA_SIZE * 2

// Utils
#define SCREEN_OFFSET(row, column) ((column) + (row) * VGA_MAX_COLS)

void kprint(char* string, uint8_t attribute);
void kprint_at(char* string, uint8_t attribute, uint16_t offset);
void set_cursor_offset(uint16_t offset);
uint16_t get_cursor_offset();
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
// [TODO] move ATR presets
#define VGA_ATR_DEFAULT (VGA_TXT_WHITE | VGA_BG_BLACK)
#define VGA_ATR_ERROR   (VGA_TXT_BLACK | VGA_BG_RED)
#define VGA_ATR_WARNING (VGA_TXT_BLACK | VGA_BG_ORANGE | VGA_BG_LIGHT)

#endif