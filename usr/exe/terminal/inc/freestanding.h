// Freestanding Commands Manager Header File // ~ eylon

#if !defined(__FREE_STANDING_H)
#define __FREE_STANDING_H

#include <stddef.h>

#define BIN_PATH "/bin/"

// Builtin command structure
typedef struct
{
    char* ref;              // command name
} cmd_freestanding_t;

const cmd_freestanding_t* freestanding_get_cmds();
size_t freestanding_get_count();
int freestanding_execute(char* argv[]);
void init_freestanding();

#endif