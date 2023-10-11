// Buzz OS Kernel Main // ~ eylon

#include <drivers/screen.h>

void kernel_main() {
	kprint("Hello from kernel", VGA_TXT_RED | VGA_BG_GREEN);
}