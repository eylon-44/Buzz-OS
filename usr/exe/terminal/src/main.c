#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

int main()
{
    char input[PATH_MAX];
    char* str[] = {"hey", "woof", NULL};
    int a = 0;

    chdir("/sys");
    getcwd(input, sizeof(input));
    printf("At: %s\n", input);

    realpath(".//..//sys/../home///", input);
    printf("Real path: %s\n", input);


    printf("Hello! World! | %s\n", input);


    struct dirent entry[4];

    int fd = open("/sys", O_RDONLY | O_DIRECTORY);

    int count = read(fd, entry, sizeof(entry)/sizeof(entry[0]));
    for (int i = 0; i < count; i++) {
        printf("%s: %s\n", entry[i].d_type == DT_REG ? "File" : "Dir", entry[i].d_name);
    }    

    close(fd);




    size_t i = 0;
    while(1)
    {
        printf("%d, ", i++);
        milisleep(500);
    }
    while (1)
    {
        printf("> ");
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