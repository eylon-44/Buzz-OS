// Built-in Commands Manager // ~ eylon

#include <builtin.h>
#include <stddef.h>

int cmd_builtin_cd(int argc, char* argv[]);
int cmd_builtin_pwd(int argc, char* argv[]);
int cmd_builtin_help(int argc, char* argv[]);

// Builtin commands list
const cmd_builtin_t cmds_builtin[] = {
    {.ref="cd", .func=cmd_builtin_cd},
    {.ref="pwd", .func=cmd_builtin_pwd},
    {.ref="help", .func=cmd_builtin_help}
};
const size_t cmds_builtin_count = sizeof(cmds_builtin)/sizeof(cmds_builtin[0]);