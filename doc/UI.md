# The UI Module
The UI module is responsible for managing and displaying the standard IO streams of each process.

The the UI module implements the following components:
- Multi-tab display
- Keyboard events handling
- STDIO interface

## Multi-tab display
A tab is a display context of a process; it is associated with the standard streams of the process and displays them. As for the VGA screen limitations, we can only display one tab at a time. Different processes with different standard streams all have their own tab, so switching between these tabs allows us to view the contents of a different process and direct our input to it.

Functionally, a tab is just a large array of characters that represent the input and output of a selected process.

```c
typedef struct tab
{
    process_t* parnet;      // orphan process
    size_t cursor;          // cursor offset in the screen
    size_t in_offset;       // offset in stdin buffer
    char in[UI_MAX_IN];     // stdin buffer
    char out[UI_MA_OUT];    // stdout buffer

    struct tab* next;
    struct tab* prev;
} tab_t;
```

The UI module keeps track of the open tabs with this structure:

```c
typedef struct
{
    tab_t* active;      // currently displayed tab
    tab_t* tab_list;    // list of all open tabs
    size_t count;       // open tabs count
} ui_t;
```

### Who Gets a Tab?
Not every process gets a tab; we want to allow children processes to be in the same tab as their parents. So who does? Only processes with no parent gets a tab, while all of their children share the same tab with them.


## Screen Layout
The standard VGA screen is built out of 25 lines of 80 characters. The first line of the screen is global and reserved, meaning that it is identical in all of the tabs and that processes can't write to it. This line is used by the UI module as a tab menu for the user; we will call it the screen header. With each new tab, a new character will apear in the screen header. These characters are used to refrence open tabs. All of the 24 lines that come after the screen header display the tab itself.

If there is only one open tab, the screen header would as follows:
> **1**

If we now open 2 more tabs and switch to the second, the screen header will update as follows:
> 1 **2** 3

Have you noticed the bold character? The bold character marks the tab that is currently displayed on the screen. Had we switched to the third tab instead, the third character would be shown in bold.

Note that the number of tabs that we can display in the screen header is limited to the size of the screen header — which is one line. We could expand the screen header to take more lines, but it seems unnecessary as for this operating system. This results that the maximum number of tabs that can be open simultaneously is limited by the number of characters we can put in the screen header.

Because switching tabs involes keyboard shortcuts as we will see later, having numbers larger than 9 in the screen header is uncomfortable both for the user and for the programer. Instead, the screen header indexing follows the keyboard structure:
> 1 2 3 4 5 6 7 8 9 0 q w e r t y u i o p a s d f g h j k l ; z x c v b n m , . /

This allows us to have 40 different characters in the screen header, which corresponds to a maximum of 40 simultaneously open tabs in the system.


## Keyboard Shortcuts
The UI module handles a few keyboard events that trigger UI and system operations.

| Keyboard Event  | Operation                                                              |
|-----------------|------------------------------------------------------------------------|
| alt+[tab]       | Switch to the tab at index [tab].                                      |
| ctrl+alt+t      | Create a new tab with a console process.                               |
| ctrl+alt+w      | Close the active tab and terminate all of its processes.               |
| ctrl+c          | Terminate all active processes of the parent of the active tab.        |
| ctrl+l          | Clear the screen.                                                      |
| [any-ascii-key] | Send the pressed key to the UI module.                                 |


## Tab Operations
As described above, each tab contains two buffers: one for storing input and one for storing output. When a process attempts to read or write to a standard file descriptor, the file system sends the request to the UI module to handle it, and the UI module updates the active tab accordingly.

### Printing: STDOUT
A process can print text to the screen by using the `write` system call, while passing to it `stdout` as the file descriptor.
> write(stdout, buff, count);

The file system will call a UI function to handle the write operation. The function checks if the caller owns the active tab. If it does, it write the string directly to the screen, but if not, it find the tab of the calling process and write the string into its output buffer.

### Inputing: STDIN
A process can get user input by using the `read` system call, while passing to it `stdin` as the file descriptor.
> read(stdin, buff, count);

The UI module is registered in the keyboard callback list. On each keyboard interrupt, the keyboard signals the UI module that a key has been pressed and passes that key to it. The UI module handles the event by writing the key to both the input buffer and the screen of the active tab. If the pressed key is a backspace, the last written key is deleted from both the input buffer and the screen. The user cannot delete more than the current input buffer, that is until the size of the input buffer reaches zero.

### Tab Creation and Destruction
Any new orphan process will automatically create a new tab. The act of creating a new tab is matter of allocating memory for the tab and adding it into the tabs list.

Killing an orphan process will automatically close its tab, and it also goes the other way around, closing a tab kills all processes in it.

### Tab Switching
Switching a tab involes copying the screen into the output buffer of the active tab, copying the output buffer of the target tab into the screen, and setting this as active.