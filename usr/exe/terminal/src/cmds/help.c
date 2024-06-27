// Built-in Terminal Command help // ~ eylon

#include <stdio.h>
#include <stddef.h>
#include <string.h>

// Command metadata structure
typedef struct
{
    char* ref;      // command name
    char* dec;      // declaration; usage
    char* desc;     // short description
} cmd_metedata_t;

static const cmd_metedata_t cmds[] = {
    {.ref="cd", .dec="cd [dirname]", .desc="Change the working directory."},
    {.ref="ls", .dec="ls [<no-arg>/pathname]", .desc="List the names and attributes of files and directories."},
    {.ref="dir", .dec="dir [dirname]", .desc="Create a new directory."},
    {.ref="file", .dec="file [filename]", .desc="Create a new file."},
    {.ref="rm", .dec="rm [pathname]", .desc="Remove a file or directory."},
    {.ref="mv", .dec="mv [oldpath] [newpath]", .desc="Move or rename a file or directory."},
    {.ref="read", .dec="read [filename]", .desc="Read a file."},
    {.ref="write", .dec="write [filename] [-w/-a](write/append) [data...", .desc="Write or append data to a file."},
    {.ref="ps", .dec="ps", .desc="Show running processes."},
    {.ref="kill", .dec="kill [pid]", .desc="Terminate a process."},
    {.ref="pr", .dec="pr [pid] [priority]", .desc="Change the priority of a process."},
    {.ref="pwd", .dec="pwd", .desc="Print working directory."},
    {.ref="help", .dec="help [<no-arg>/command]", .desc="Get information about commands."},
    {.ref="echo", .dec="echo [text...", .desc="Print to the screen."},
    {.ref="clear", .dec="clear", .desc="Clear the screen."}
};

// Supply information about terminal commands
int cmd_builtin_help(int argc, char* argv[])
{
    // If no arguments were given
    if (argc < 2) {
        printf(" - Usage: help [<no-arg>/command]\n");
        printf(" Avialable commands:\n");
        for (size_t i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++)
        {
            printf("  * %s\n", cmds[i].dec);
        }
        return 0;
    }
    // If an argument was given
    for (size_t i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++)
    {
        if (strcmp(argv[1], cmds[i].ref) == 0) {
            printf(" - Usage: %s\n %s\n", cmds[i].dec, cmds[i].desc);
            return 0;
        }
    }
    printf(" - No such command \"%s\".\n - For a list of avialable commands, use the \"help\" command.\n", argv[1]);

    return 0;
}