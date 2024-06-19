#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

int main()
{
    char input[128];
    char* str[] = {"hey", "wow", NULL};

    while (1)
    {
        input[syscall(SYS_read, stdin, input, 128)] = '\0';
        printf("%s\n", input);
        if (strcmp(input, "echo") == 0) {
            execve("/bin/echo.elf", str);
        }

    }

    while (1) {}
}