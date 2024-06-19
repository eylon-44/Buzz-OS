#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char inp[64];
    strcpy(inp, argv[0]);
    
    // printf("%s\n", argv[0]);
    for (int i = 0; i < 2; i++)
    {
        // syscall(SYS_write, STDOUT_FILENO, "SYSboi", 5);
        printf("text1 %s", (char*) argv[0]);
    }
    return 0;
}