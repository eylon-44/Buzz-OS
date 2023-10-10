// VGA Screen Driver // ~ eylon

#include <drivers/screen.h>
#include <drivers/ports.h>
#include <utils/type.h>

// Screen settings
#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS    25
#define MAX_COLS    80
#define SCREEN_SIZE (MAX_ROWS * MAX_COLS)

// Attribute byte for default color scheme
# define DEFAULT_TXT_COLOR (VGA_TXT_WHITE | VGA_BG_BLACK)

// Screen I/O ports
#define SCREEN_CTRL_PORT 0x3d4
#define SCREEN_DATA_PORT 0x3d5


// Scroll the screen down (move the text up) without saving the lost text
// # also scrolls the cursor
// TODO edit P70
static void scroll_down()
{
    u8_t* vidmem = (u8_t*) VIDEO_ADDRESS;

    // for each row, starting from the second
    for (u8_t r = 1; r < MAX_ROWS; r++)
    {
        // for each column in a row
        for (u8_t c = 0; c < MAX_COLS; c++)
        {
            // copy the current column in a row to the column in the row before it
            vidmem[ (MAX_ROWS*(r-1) + c) * 2 ]    = (u16_t) vidmem[ (MAX_ROWS*r + c) * 2 ];

            // if it's the last row, clean it
            if (r == MAX_ROWS-1) {
                vidmem[ (MAX_ROWS*r + c) * 2 ] = (u16_t) 0x00000000;
            }
        }
    }

    // scroll the cursor
    set_cursor_offset(get_cursor_offset() - MAX_COLS * 2);
}

// Print a single character to the screen at the current cursor location
static void print_char(char character, u8_t attribute)
{
    unsigned char* vidmem = (unsigned char*) VIDEO_ADDRESS;
    u16_t offset = get_cursor_offset();

    // scroll down if overwritten to the screen
    if (offset/2 > SCREEN_SIZE) {
        scroll_down();
        offset -= MAX_COLS * 2;
    }

    // set the character
    vidmem[offset]   = character;
    vidmem[offset+1] = attribute;
}

// Print a null terminated string
void kprint(char* string, u8_t attribute)
{
    u16_t i = 0;
    // keep printing until the null character
    while (string[i] != 0)
    {
        print_char(string[i], attribute);
        i++;
    }
}

// Print a null terminated string to the screen at a specific cursor offset
// # offset < 0 will print at the current cursor location
void kprint_at(char* string, u8_t attribute, u16_t offset)
{
    if (offset >= 0) set_cursor_offset(offset);
    kprint(string, attribute);
}

// Use VGA ports to set cursor offset inside video memory
void set_cursor_offset(u16_t offset)
{
    offset /= 2;

    // control port = 0x0E -> data port = high byte
    port_byte_out(SCREEN_CTRL_PORT, 0x0E);
    port_byte_out(SCREEN_DATA_PORT, (u8_t) (offset >> 8) & 0xFFFF); // high

    // control port = 0x0F -> data port = low byte
    port_byte_out(SCREEN_CTRL_PORT, 0x0F);
    port_byte_out(SCREEN_DATA_PORT, (u8_t) (offset & 0xFFFF)); // low
}

// Use VGA ports to get cursor offset inside video memory
u16_t get_cursor_offset()
{
    // control port = 0x0E -> data port = high byte
    port_byte_out(SCREEN_CTRL_PORT, 0x0E);
    u16_t offset = port_byte_in(SCREEN_DATA_PORT) << 8; // high byte << 8
    
    // control port = 0x0F -> data port = low byte
    port_byte_out(SCREEN_CTRL_PORT, 0x0F);
    offset |= port_byte_in(SCREEN_DATA_PORT);

    // return [position * size of character cell in video memory]
    return offset * 2;
}

// Clear the screen and reset the cursor
void clear_screen()
{
    unsigned char* vidmem = (unsigned char*) VIDEO_ADDRESS;

    // fill the screen with a blank character (space)
    for (int i = 0; i < SCREEN_SIZE; i++) 
    {
        vidmem[i*2]     = ' ';
        vidmem[(i*2)+1] = DEFAULT_TXT_COLOR;
    }

    // reset cursor offset to 0
    set_cursor_offset(0);
}