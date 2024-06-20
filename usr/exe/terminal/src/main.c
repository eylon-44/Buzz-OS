#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

int main()
{
    char input[128];
    char* str[] = {"hey", "wow", NULL};
    int a = 0;

    printf("Hello! World!\n");
    size_t i = 0;
    // while(1)
    // {
    //     printf("%d\n", i);
    //     i++;
    //     milisleep(500);
    // }
    while (1)
    {
        input[syscall(SYS_read, stdin, input, 128)] = '\0';
        printf("%s\n", input);
        if (strcmp(input, "echo") == 0) {
            execve("/bin/echo.elf", str);
        }
        if (strcmp(input, "bug") == 0) {
            *(char*) 0xFFFFFFFF = 'a';
        }

    }

    while (1) {}
}