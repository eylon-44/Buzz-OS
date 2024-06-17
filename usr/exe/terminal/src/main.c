#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>

extern char end, edata;

// int a = 10;
// const int b = 20;
// int c;

int main()
{
    // c = 10;
    char result[] = " | You inputed: ";

    char* input = malloc(128);
    free(input);
    
    syscall(SYS_write, 1, "Input: ", 6);
    input[syscall(SYS_read, 0, input, 128)] = '\0';
    syscall(SYS_write, 1, result, strlen(result));
    syscall(SYS_write, 1, input, strlen(input));



    while (1) {}

	return 0;
}