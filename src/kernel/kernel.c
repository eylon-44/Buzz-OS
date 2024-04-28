// Buzz OS Kernel Main // ~ eylon

#include <kernel/memory/mm.h>
#include <kernel/process/pm.h>
#include <kernel/panic.h>
#include <kernel/syscall/syscall.h>
#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <cpu/interrupts/isr.h>
#include <cpu/timer.h>

// Kernel main function :: kernel start
void kernel_main() 
{
    init_mm();
    init_interrupt();
    init_syscall();
    init_keyboard();
    init_timer();
    init_pm();

    // Print a welcome message
    clear_screen();
    kprint(". . . Welcome to Buzz OS . . .", VGA_BG_ORANGE | VGA_TXT_BLACK);
    kprint("\n. . . Loading the system for you; please wait . . .", VGA_ATR_DEFAULT);


    // Set the interrupt flag; enable interrupts
    __asm__ volatile ("sti");

	// Halt forever; wait for an interrupt, handle it, continue halting
	for (;;) { __asm__ __volatile__ ("hlt"); }

	// This code should never run
	KPANIC("KERNEL: REACHED END OF KERNEL");
}