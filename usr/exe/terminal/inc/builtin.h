// Built-in Commands Manager Header File //

#if !defined(__BUILTIN_H)
#define __BUILTIN_H

#include <stddef.h>

// Builtin command structure
typedef struct
{
    char* ref;                              // command name
    int (*func)(int argc, char* argv[]);    // handler function
} cmd_builtin_t;

extern const cmd_builtin_t cmds_builtin[];
extern const size_t cmds_builtin_count;

#endif