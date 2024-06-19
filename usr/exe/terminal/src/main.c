#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char* input = malloc(128);

    printf("Enter your name: ");
    input[syscall(SYS_read, stdin, input, 128)] = '\0';

    printf(" Hello %s", input);

    while (1) {}
}