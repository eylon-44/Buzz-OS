// VGA Screen Driver // ~ eylon

#include <drivers/screen.h>
#include <drivers/ports.h>
#include <libc/stdint.h>
#include <libc/stddef.h>
#include <libc/string.h>

// Screen I/O ports
#define SCREEN_CTRL_PORT 0x3d4
#define SCREEN_DATA_PORT 0x3d5

static uint8_t* vidmem = (uint8_t*) VGA_VIRT_MEM;


// Process an escape sequence character :: escape sequence characters are not printed and only affect the cursor
static uint16_t handle_escape_sequence(UNUSED char character, uint16_t offset)
{
    // [TODO]
    return offset;
}

// Put a single character on the screen at the given offset
static void put_char(char character, uint8_t attribute, uint16_t offset)
{
    // put the character on screen
    vidmem[(offset*2)]   = character;
    vidmem[(offset*2)+1] = attribute;
}

// Scroll the screen down (move the text up) :: return the new offset :: also scrolls the cursor
static uint16_t handle_scrolling(uint16_t offset)
{
    // if going out of screen bounds scroll the screen
    if (offset > VGA_SIZE) {
        memcpy(vidmem, vidmem + VGA_MAX_COLS * 2, (VGA_SIZE - VGA_MAX_COLS)*2);
        // blank the last line
        for (uint8_t i = 0; i < VGA_MAX_COLS; i++) {
            put_char(' ', VGA_ATR_DEFAULT, (VGA_SIZE - VGA_MAX_COLS + i));
        }
        offset -= (VGA_MAX_COLS + 1);
    }
    return offset;
}


// Print a null terminated string at the current cursor locaion :: attribute 0 for default
void kprint(char* string, uint8_t attribute)
{
    if (attribute == 0) { attribute = VGA_ATR_DEFAULT; }

    // get cursor offset :: starting the print from the cursor location
    uint16_t offset = get_cursor_offset();

    uint16_t i = 0;
    // keep printing until the null character
    while (string[i] != '\0') {

            offset = handle_escape_sequence(string[i], offset);
            offset = handle_scrolling(offset);
            put_char(string[i], attribute, offset); // print the character
    
            i++;
            offset++;
    }

    // update cursor location
    set_cursor_offset(offset);
}

// Print a null terminated string to the screen at a specific offset
void kprint_at(char* string, uint8_t attribute, uint16_t offset)
{
    set_cursor_offset(offset);
    kprint(string, attribute);
}

// Use VGA ports to set cursor offset inside video memory
void set_cursor_offset(uint16_t offset)
{
    // control port = 0x0E -> data port = high byte
    port_outb(SCREEN_CTRL_PORT, 0x0E);
    port_outb(SCREEN_DATA_PORT, (uint8_t) (offset >> 8) & 0xFFFF); // high

    // control port = 0x0F -> data port = low byte
    port_outb(SCREEN_CTRL_PORT, 0x0F);
    port_outb(SCREEN_DATA_PORT, (uint8_t) (offset & 0xFFFF)); // low
}

// Use VGA ports to get cursor offset inside video memory
uint16_t get_cursor_offset()
{
    // control port = 0x0E -> data port = high byte
    port_outb(SCREEN_CTRL_PORT, 0x0E);
    uint16_t offset = port_inb(SCREEN_DATA_PORT) << 8; // high byte << 8
    
    // control port = 0x0F -> data port = low byte
    port_outb(SCREEN_CTRL_PORT, 0x0F);
    offset |= port_inb(SCREEN_DATA_PORT);

    // return
    return offset;
}

// Clear the screen and reset the cursor
void clear_screen()
{
    // fill the screen with a blank character (space)
    for (int i = 0; i < VGA_SIZE; i++)
    {
        vidmem[i*2]     = ' ';
        vidmem[(i*2)+1] = VGA_ATR_DEFAULT;
    }

    // reset cursor offset to 0
    set_cursor_offset(0);
}