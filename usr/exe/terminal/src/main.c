#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

int main()
{
    char input[128] = "Input: ";
    char result[] = " | You inputed: ";

    syscall(SYS_write, 1, input, strlen(input));
    input[syscall(SYS_read, 0, input, sizeof(input))] = '\0';
    syscall(SYS_write, 1, result, strlen(result));
    syscall(SYS_write, 1, input, strlen(input));
    while (1) {}

	return 0;
}