// UI Manager // ~ eylon

#include <kernel/ui.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/process/pm.h>
#include <kernel/process/scheduler.h>
#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <libc/sys/syscall.h>
#include <libc/unistd.h>
#include <libc/string.h>
#include <libc/list.h>

static tab_list_t tabs = {.tab_list=NULL, .active=NULL, .active_index=0, .count=0};
static const char tab_indexes[UI_MAX_TABS+1] = "1234567890qwertyuiopasdfghjkl;zxcvbnm,./";
static char stdin_buff[UI_MAX_IN];

static int get_index_by_char(char chr_indx)
{    
    size_t lst_indx = (int) strchr(tab_indexes, chr_indx) - (int) tab_indexes;
    if (lst_indx > UI_MAX_TABS || tabs.count <= lst_indx) {
        return -1;
    }
    return (int) lst_indx;
}

static tab_t* get_tab_by_index(char index)
{
    tab_t* tab = tabs.tab_list;
    int list_index = get_index_by_char(index);

    if (list_index < 0) {
        return NULL;
    }

    // Find the tab at the given index
    while (list_index > 0)
    {
        tab = tab->next;
        list_index--;
    }

    return tab;
}

// Print the screen header
static void update_header()
{
    // Go over all tab indexes
    for (size_t i = 0; i < UI_MAX_TABS; i++)
    {
        if (i < tabs.count) {
            // Print active tab
            if (tab_indexes[i] == tabs.active_index) {
                vga_put_char_at(tab_indexes[i], UI_ATR_TAB_SELECTED, i*2);
            }
            // Print opened tab
            else {
                vga_put_char_at(tab_indexes[i], UI_ATR_TAB_OPEN, i*2);
            }
        }
        // Print closed tab
        else {
            vga_put_char_at(tab_indexes[i], UI_ATR_TAB_CLOSE, i*2);
        }

        // Space the indexing
        vga_put_char_at(' ', UI_ATR_TAB_CLOSE, i*2+1);
    }
}

/* Scroll the screen down (move the text up) so it could fit [count] bytes and return the new offset.
    [buff_out] is the screen buffer; could be the VGA screen or any other stdout buffer.
    [offset] is the current cursor location on the buffer.
    [count] is the number of bytes we try to fit in the screen by scrolling.
*/
static size_t handle_scrolling(char* buff_out, int offset, size_t count)
{
    // If print is going out of screen bounds
    while (offset+count >= UI_MAX_OUT) {
        memcpy(buff_out, buff_out + VGA_COL_COUNT * 2, (VGA_SIZE - VGA_COL_COUNT)*2);
        // blank the last line
        for (uint8_t i = 0; i < VGA_COL_COUNT; i++) {
            vga_put_char_at(' ', UI_ATR_DEFAULT, (VGA_SIZE - VGA_COL_COUNT + i));
        }
        offset -= (VGA_COL_COUNT);
    }
    if (offset < 0) {
        return 0;
    }
    return offset;
}



static void handle_shortcut(UNUSED char key, UNUSED uint8_t modifiers)
{
//TODO
}

// Keyboard event handler
void ui_key_event_handler(char key, uint8_t modifiers)
{
    // If there are no open tabs, return
    if (tabs.count <= 0) {
        return;
    }

    // If event is a shortcut, pass it to the shorcut handler and return
    if (modifiers & (KB_MFLAG_CTRL | KB_MFLAG_ALT)) {
        handle_shortcut(key, modifiers);
        return;
    }

    // If backspace was pressed
    if (key == KEY_BACKSPACE) {
        // If buffer is not empty
        if (tabs.active->in_offset > 0)
        {
            // Delete the last character
            ui_cursor_set(tabs.active, ui_cursor_get(tabs.active)-1);
            vga_put_char_at(' ', UI_ATR_DEFAULT, ui_cursor_get(tabs.active) + VGA_COL_COUNT);
            // Decrease the offset
            tabs.active->in_offset--;
        }
        return;
    }
    // If return was pressed
    if (key == KEY_RETURN) {
        // flush
        return;
    }

    // If buffer is full
    if (tabs.active->in_offset >= UI_MAX_IN) {
        return;
    }

    // Add key to the buffer
    stdin_buff[tabs.active->in_offset] = key;
    tabs.active->in_offset++;

    // Print the key ???with stdout????
    ui_cursor_set(tabs.active, handle_scrolling((char*) UI_SCREEN_BUFF, ui_cursor_get(tabs.active), 1));
    vga_put_char(key, UI_ATR_DEFAULT);
}

// BOTH OF THESE FUNCTIONS NEED TO HAVE THE CALLING PROCESS HAS A PARAMETER
void ui_stdin_read(UNUSED const char* buff, UNUSED size_t count)
{
    UNUSED tab_t* tab = pm_get_active()->tab;
    sched_set_status(pm_get_active(), TS_BLOCKED);
    syscall(SYS_sched_yield);

    /* possible solution:
        the caller for this function must be of the same address space. what if we block the current address
        space and yield, meaning that we force a task switch. another process, (who? what if we have only 1) will
        handle the keyboard event and when it comes the time to flush it will simply put this context back to work. */
    /* reset stdin buffer, block the calling process, resume it only after a flush. */
}

// Write to the stdout buffer of the calling process
void ui_stdout_write(const char* buff, size_t count)
{
    tab_t* tab = pm_get_active()->tab;

    // If the tab is the active tab, write directly to the screen
    if (tab == tabs.active) {
        // Scroll the screen as needed        
        ui_cursor_set(tab, handle_scrolling((char*) UI_SCREEN_BUFF, ui_cursor_get(tab), count));
        // Print the string to the screen
        vga_print_n(buff + count - (count%UI_MAX_OUT), UI_ATR_DEFAULT, count % UI_MAX_OUT);
    }
    // If not, write to its buffer
    else {
        // Temporarlily attach the buffer of the target tab
        tab_buff_t* buff = (tab_buff_t*) vmm_attach_page((paddr_t) tab->buff);

        // Scroll the screen as needed        
        tab->out_offset = handle_scrolling(buff->out, tab->out_offset, count);
        // Copy add the string into the buffer
        memcpy(buff->out, buff + count - (count%UI_MAX_OUT), count % UI_MAX_OUT);

        vmm_detach_page((vaddr_t) buff);
    }
}

// Set the cursor for the given tab
void ui_cursor_set(tab_t* tab, size_t offset)
{
    // If it's the active tab
    if (tabs.active == tab) {
        vga_set_cursor(offset + VGA_COL_COUNT);
    }
    else {
        tab->out_offset = offset;
    }
    
}

// Set the cursor of the given tab
size_t ui_cursor_get(tab_t* tab)
{
    // If it's the active tab
    if (tabs.active == tab) {
        return vga_get_cursor() - VGA_COL_COUNT;
    }
    else {
        return tab->out_offset;
    }
}

// Open a new tab with the default program and switch to it
void ui_tab_open(process_t* parent)
{
    tab_t* tab;
    tab_buff_t* buff;

    // Allocate and set the new tab
    tab = (tab_t*) kmalloc(sizeof(tab_t));
    tab->out_offset = 0;
    tab->in_offset  = 0;
    tab->parnet     = parent;
    tab->buff       = (tab_buff_t*) pmm_get_page();

    // Clear stdout buffer
    buff = (tab_buff_t*) vmm_attach_page((paddr_t) tab->buff);
    memset(buff->out, ' ', UI_MAX_OUT);
    vmm_detach_page((vaddr_t) buff);

    // Add the tab to the list
    LIST_ADD_END(tabs.tab_list, tab);
    tabs.count++;

    // Switch to the new tab
    ui_tab_switch(tab_indexes[tabs.count-1]);
}

// Close the displayed tab
void ui_tab_close()
{
    // Free tab resources and remove it from the list
    pmm_free_page((paddr_t) tabs.active->buff);
    LIST_REMOVE(tabs.tab_list, tabs.active);
    kfree(tabs.active);
    tabs.active = NULL;

    // Decrease tab count
    tabs.count--;

    // If there are tabs left open, switch to [TODO]
    if (tabs.count > 0) {
        ui_tab_switch('1');
    }
    // Else, update the header
    else {
        update_header();
    }
}

// Switch displayed tab
void ui_tab_switch(char index)
{
    tab_t* tab = get_tab_by_index(index);
    tab_buff_t* buff;

    // If there is no tab at the given index, or switching to the active tab, return
    if (tab == NULL || tab == tabs.active) {
        return;
    }

    // If there is an open tab, save its contents before switching
    if (tabs.active != NULL) {
        // Save the stdin and stdout buffer
        buff = (tab_buff_t*) vmm_attach_page((paddr_t) tabs.active->buff);
        for (int i = 0; i < UI_MAX_OUT; i++) {
            buff->out[i] = ((char*) UI_SCREEN_BUFF)[i*2];
        }
        memcpy(buff->in, stdin_buff, tabs.active->in_offset+1);
        vmm_detach_page((vaddr_t) buff);

        // Save the cursor
        tabs.active->out_offset = ui_cursor_get(tabs.active);
    }

    // Set the tab as active
    tabs.active       = tab;
    tabs.active_index = index;

    // Set the cursor
    ui_cursor_set(tabs.active, tab->out_offset);

    // Temporarlily attach the buffer of the target tab
    buff = (tab_buff_t*) vmm_attach_page((paddr_t) tab->buff);

    // Copy the stdout buffer to the screen
    for (int i = 0; i < UI_MAX_OUT; i++) {
        ((char*) UI_SCREEN_BUFF)[i*2]   = buff->out[i];
        ((char*) UI_SCREEN_BUFF)[i*2+1] = UI_ATR_DEFAULT;
    }

    // Copy the stdin buffer into the local buffer
    memcpy(stdin_buff, buff->in, tabs.active->in_offset+1);

    // Detach the buffer and update the header
    vmm_detach_page((vaddr_t) buff);
    update_header();
}

// Initiate the UI module
void init_ui()
{
    vga_clear();

    process_t* p = pm_load(NULL, "/sys/itest.elf", 2000);
    ui_tab_open(p);
    p->tab = tabs.active;
}