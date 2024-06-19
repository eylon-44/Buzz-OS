// VGA Screen Driver // ~ eylon

#include <drivers/screen.h>
#include <drivers/ports.h>
#include <kernel/ui.h>
#include <libc/stdint.h>
#include <libc/stddef.h>
#include <libc/string.h>

// Screen I/O ports
#define SCREEN_CTRL_PORT 0x3d4
#define SCREEN_DATA_PORT 0x3d5

static uint8_t* vidmem = (uint8_t*) VGA_VIRT_MEM;

// Put a single character on the screen at the current cursor location
inline void vga_put_char(char character, uint8_t attribute)
{
    // Print the character at the current cursor location
    size_t offset = vga_put_char_at(character, attribute, vga_get_cursor());
    // Update cursor location
    vga_set_cursor(offset);
}

/* Put a single character on the screen at a given offset.
    Returns the screen offset at which the string terminates. */
inline size_t vga_put_char_at(char character, uint8_t attribute, size_t offset)
{
    if (attribute == 0) { attribute = UI_ATR_DEFAULT; }

    // Put the character on screen
    vidmem[(offset*2)]   = character;
    vidmem[(offset*2)+1] = attribute;

    return offset+1;
}

// Print a null terminated string at the current cursor location and move the cursor to the end of it
void vga_print(const char* string, uint8_t attribute)
{
    // Print the string at the current cursor location
    size_t offset = vga_print_at(string, attribute, vga_get_cursor());
    // Update cursor location
    vga_set_cursor(offset);
}

/* Print a null terminated string to the screen at a given offset.
    Returns the screen offset at which the string terminates. */
size_t vga_print_at(const char* string, uint8_t attribute, size_t offset)
{
    // Iterate over the string and print each character
    for (size_t i = 0; string[i] != '\0'; i++) {
        offset = vga_put_char_at(string[i], attribute, offset);
    }

    return offset;
}

// Print a null terminated string at the current cursor location and move the cursor to the end of it
void vga_print_n(const char* string, uint8_t attribute, size_t n)
{
    size_t offset = vga_print_at_n(string, attribute, vga_get_cursor(), n);
    // Update cursor location
    vga_set_cursor(offset);
}

/* Print [n] bytes from [string] to the screen at a given offset.
    Returns the screen offset at which the string terminates. */
size_t vga_print_at_n(const char* string, uint8_t attribute, size_t offset, size_t n)
{
    // Iterate over the string and print each character
    for (size_t i = 0; i < n; i++) {
        offset = vga_put_char_at(string[i], attribute, offset);
    }

    return offset;
}

// Use VGA ports to set cursor offset inside video memory
void vga_set_cursor(size_t offset)
{
    // control port = 0x0E -> data port = high byte
    port_outb(SCREEN_CTRL_PORT, 0x0E);
    port_outb(SCREEN_DATA_PORT, (uint8_t) (offset >> 8) & 0xFFFF); // high

    // control port = 0x0F -> data port = low byte
    port_outb(SCREEN_CTRL_PORT, 0x0F);
    port_outb(SCREEN_DATA_PORT, (uint8_t) (offset & 0xFFFF)); // low
}

// Use VGA ports to get cursor offset inside video memory
size_t vga_get_cursor()
{
    // control port = 0x0E -> data port = high byte
    port_outb(SCREEN_CTRL_PORT, 0x0E);
    size_t offset = port_inb(SCREEN_DATA_PORT) << 8; // high byte << 8
    
    // control port = 0x0F -> data port = low byte
    port_outb(SCREEN_CTRL_PORT, 0x0F);
    offset |= port_inb(SCREEN_DATA_PORT);

    // return
    return offset;
}

// Clear the screen and reset the cursor
void vga_clear()
{
    // Fill the screen with spaces
    for (int i = 0; i < VGA_SIZE; i++)
    {
        vidmem[i*2]     = ' ';
        vidmem[(i*2)+1] = UI_ATR_DEFAULT;
    }

    // Reset cursor
    vga_set_cursor(0);
}