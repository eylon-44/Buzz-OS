#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

int main()
{
    char* str = "Hello, I am a user program calling a kernel routine!";
    syscall(SYS_write, 1, str, strlen(str));
    while(1) {};
	return 0;
}