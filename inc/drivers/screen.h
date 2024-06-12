// Screen driver header file // ~ eylon

#if !defined(SCREEN__DRIVER_H)
#define SCREEN_DRIVER_H

#include <kernel/memory/mmlayout.h>
#include <libc/stdint.h>
#include <libc/stddef.h>

// Screen measurements
#define VGA_ROW_COUNT 25
#define VGA_COL_COUNT 80
#define VGA_SIZE (VGA_ROW_COUNT * VGA_COL_COUNT)

// Video memory
#define VGA_VIRT_MEM MM_MMIO_START
#define VGA_PHYS_MEM 0x000B8000
#define VGA_MEM_SIZE VGA_SIZE * 2

// Utils
#define SCREEN_OFFSET(row, column) ((column) + (row) * VGA_COL_COUNT)

void vga_print_n(const char* string, uint8_t attribute, size_t n);
size_t vga_print_at_n(const char* string, uint8_t attribute, size_t offset, size_t n);
void vga_put_char(char character, uint8_t attribute);
size_t vga_put_char_at(char character, uint8_t attribute, size_t offset);
void vga_print(const char* string, uint8_t attribute);
size_t vga_print_at(const char* string, uint8_t attribute, size_t offset);
void vga_set_cursor(size_t offset);
size_t vga_get_cursor();
void vga_clear();

/* VGA attribute table */

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

/* Keys */
#define KEY_BACKSPACE   0x08
#define KEY_RETURN      0x0A

#endif