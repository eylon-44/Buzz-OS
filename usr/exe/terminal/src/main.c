// Default Terminal // ~ eylon

#include <input.h>
#include <builtin.h>
#include <freestanding.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

// Working directory string
char wd[PATH_MAX];
// Argument array to be passed to the handling command
char* argv[ARGC_MAX];

/* The default user terminal. */
int main()
{
    init_freestanding();
    getcwd(wd, PATH_MAX);

    for (;;)
    {
        int argc;

        // Read command
        printf("%s :: ", wd);
        argc = get_input(argv);

        // No command
        if (argc == 0) {
            continue;
        }

        // If seeking for a file, try to execute it
        if (strchr(argv[0], '/') != NULL) {
            char path[PATH_MAX];
            if (realpath(argv[0], path) != NULL) {
                execve(path, argv);
                goto loop_end;
            }
        }

        // Search for a matching built-in command
        for (size_t i = 0; i < cmds_builtin_count; i++) {
            if (strcmp(argv[0], cmds_builtin[i].ref) == 0) {
                cmds_builtin[i].func(argc, argv);
                goto loop_end;
            }
        }

        // Search for a matching freestanding command
        for (size_t i = 0; i < freestanding_get_count(); i++) {
            if (strcmp(argv[0], freestanding_get_cmds()[i].ref) == 0) {
                freestanding_execute(argv);
                goto loop_end;
            }
        }

        
        // Command not found
        printf("Command \"%s\" not found.\n", argv[0]);

        loop_end:
            free_input(argv);
    }
}