// VGA Screen Driver // ~ eylon

#include <drivers/screen.h>
#include <drivers/ports.h>
#include <utils/type.h>
#include <utils/memory.h>

// Screen settings
#define VIDEO_ADDRESS 0xb8000

// Screen I/O ports
#define SCREEN_CTRL_PORT 0x3d4
#define SCREEN_DATA_PORT 0x3d5

static u8_t* vidmem = (u8_t*) VIDEO_ADDRESS;


// Process an escape sequence character :: escape sequence characters are not printed and only affect the cursor
static u16_t handle_escape_sequence(char character, u16_t offset)
{
    // [TODO]
    UNUSED(character);
    return offset;
}

// Put a single character on the screen at the given offset
static void put_char(char character, u8_t attribute, u16_t offset)
{
    // put the character on screen
    vidmem[(offset*2)]   = character;
    vidmem[(offset*2)+1] = attribute;
}

// Scroll the screen down (move the text up) :: return the new offset :: also scrolls the cursor
static u16_t handle_scrolling(u16_t offset)
{
    // if going out of screen bounds scroll the screen
    if (offset > SCREEN_SIZE) {
        memcpy(vidmem + SCREEN_MAX_COLS * 2, vidmem, (SCREEN_SIZE - SCREEN_MAX_COLS)*2);
        // blank the last line
        for (u8_t i = 0; i < SCREEN_MAX_COLS; i++) {
            put_char(' ', VGA_ATR_DEFAULT, (SCREEN_SIZE - SCREEN_MAX_COLS + i));
        }
        offset -= (SCREEN_MAX_COLS + 1);
    }
    return offset;
}


// Print a null terminated string at the current cursor locaion :: attribute 0 for default
void kprint(char* string, u8_t attribute)
{
    if (attribute == 0) { attribute = VGA_ATR_DEFAULT; }

    // get cursor offset :: starting the print from the cursor location
    u16_t offset = get_cursor_offset();

    u16_t i = 0;
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
void kprint_at(char* string, u8_t attribute, u16_t offset)
{
    set_cursor_offset(offset);
    kprint(string, attribute);
}

// Use VGA ports to set cursor offset inside video memory
void set_cursor_offset(u16_t offset)
{
    // control port = 0x0E -> data port = high byte
    port_outb(SCREEN_CTRL_PORT, 0x0E);
    port_outb(SCREEN_DATA_PORT, (u8_t) (offset >> 8) & 0xFFFF); // high

    // control port = 0x0F -> data port = low byte
    port_outb(SCREEN_CTRL_PORT, 0x0F);
    port_outb(SCREEN_DATA_PORT, (u8_t) (offset & 0xFFFF)); // low
}

// Use VGA ports to get cursor offset inside video memory
u16_t get_cursor_offset()
{
    // control port = 0x0E -> data port = high byte
    port_outb(SCREEN_CTRL_PORT, 0x0E);
    u16_t offset = port_inb(SCREEN_DATA_PORT) << 8; // high byte << 8
    
    // control port = 0x0F -> data port = low byte
    port_outb(SCREEN_CTRL_PORT, 0x0F);
    offset |= port_inb(SCREEN_DATA_PORT);

    // return
    return offset;
}

// Clear the screen and reset the cursor
void clear_screen()
{
    unsigned char* vidmem = (unsigned char*) VIDEO_ADDRESS;

    // fill the screen with a blank character (space)
    for (int i = 0; i < SCREEN_SIZE; i++)
    {
        vidmem[i*2]     = ' ';
        vidmem[(i*2)+1] = VGA_ATR_DEFAULT;
    }

    // reset cursor offset to 0
    set_cursor_offset(0);
}