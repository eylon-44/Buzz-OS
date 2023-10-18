// Buzz OS Kernel Main // ~ eylon

#include <drivers/screen.h>
#include <cpu/interrupts/isr.h>
#include <cpu/timer.h>

void kernel_main() {
	kprint("Hello from kernel", VGA_TXT_RED | VGA_BG_GREEN);
	init_interrupt();
	//init_timer(20);

	//__asm__ __volatile__ ("int $32");

	__asm__ __volatile__ ("hlt");

	kprint("End of kernel", VGA_TXT_RED | VGA_BG_BLUE);
}