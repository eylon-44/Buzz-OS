// UI Manager Header File // ~ eylon

#if !defined(UI_H)
#define UI_H

#include <kernel/process/pm.h>
#include <drivers/screen.h>
#include <libc/stddef.h>
#include <libc/stdint.h>

#define UI_MAX_TABS (40)
#define UI_MAX_IN   ((VGA_ROW_COUNT-2) * VGA_COL_COUNT)
#define UI_MAX_OUT  ((VGA_ROW_COUNT-1) * VGA_COL_COUNT)

/* Print color presets */
#define UI_BG_DEFAULT       (VGA_BG_BLACK)
#define UI_TXT_DEFAULT      (VGA_TXT_WHITE)
#define UI_ATR_DEFAULT      (UI_TXT_DEFAULT | UI_BG_DEFAULT)
#define UI_ATR_ERROR        (VGA_TXT_BLACK | VGA_BG_RED)
#define UI_ATR_WARNING      (VGA_TXT_BLACK | VGA_BG_ORANGE | VGA_BG_LIGHT)
#define UI_ATR_TAB_OPEN     (VGA_TXT_WHITE | UI_BG_DEFAULT)
#define UI_ATR_TAB_CLOSE    (VGA_TXT_BLACK | VGA_TXT_LIGHT | UI_BG_DEFAULT)
#define UI_ATR_TAB_SELECTED (VGA_TXT_WHITE | VGA_BG_RED)

#define UI_HEADER_BUFF  (VGA_VIRT_MEM)
#define UI_SCREEN_BUFF  (VGA_VIRT_MEM + VGA_COL_COUNT*2)

#define UI_DEFAULT_TERMINAL "/sys/terminal.elf"

// Tab Flags
#define TABF_TAKING_INPUT 0x01

// Tab data structure
typedef struct
{
    char in[UI_MAX_IN];     // stdin buffer
    char out[UI_MAX_OUT];   // stdout buffer
} tab_buff_t;

// Tab structure
typedef struct tab
{
    struct process* parnet;     // orphan process of tab
    size_t in_offset;           // offset in stdin buffer
    size_t out_offset;          // offset in stdout buffer; cursor offset
    tab_buff_t* buff;           // physical address of tab stdin and stdout buffer

    uint32_t flags;

    struct tab* next;
    struct tab* prev;
} tab_t;

// Tab list structure
typedef struct
{
    tab_t* tab_list;        // list of all open tabs
    tab_t* active;          // currently displayed tab
    int active_index;       // screen header index of active tab
    int count;              // number of open tabs
} tab_list_t;

void ui_key_event_handler(char key, uint8_t modifiers);
size_t ui_stdin_read(UNUSED const char* buff, UNUSED size_t count);
ssize_t ui_stdout_write(const char* buff, size_t count);
void ui_cursor_set(tab_t* tab, size_t offset);
size_t ui_cursor_get(tab_t* tab);
void ui_tab_open();
void ui_tab_close();
void ui_tab_switch(char index);
void init_ui();

#endif