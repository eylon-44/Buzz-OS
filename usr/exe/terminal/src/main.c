#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>

extern char end, edata;

int main()
{
    char end_str[16];
    itoa((size_t) &edata, end_str);
    char input[128] = "Input: ";
    char result[] = " | You inputed: ";

    syscall(SYS_write, 1, input, strlen(input));
    input[syscall(SYS_read, 0, input, sizeof(input))] = '\0';
    syscall(SYS_write, 1, result, strlen(result));
    syscall(SYS_write, 1, input, strlen(input));

    syscall(SYS_write, 1, end_str, strlen(end_str));
    while (1) {}

	return 0;
}