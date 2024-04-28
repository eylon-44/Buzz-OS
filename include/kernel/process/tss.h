// TSS Header File // ~ eylon

#if !defined(TSS_H)
#define TSS_H

#include <libc/stddef.h>

// TSS data structure
typedef struct {
	uint32_t prev_tss;      // unused
	uint32_t esp0;          // stack pointer to load when changing to ring 0
	uint32_t ss0;           // stack segment to load when changing to ring 0
	uint32_t esp1;          // unused
	uint32_t ss1;           // unused
	uint32_t esp2;          // unused
	uint32_t ss2;           // unused
	uint32_t cr3;           // unused
	uint32_t eip;           // unused
	uint32_t eflags;        // unused
	uint32_t eax;           // unused
	uint32_t ecx;           // unused
	uint32_t edx;           // unused
	uint32_t ebx;           // unused
	uint32_t esp;           // unused
	uint32_t ebp;           // unused
	uint32_t esi;           // unused
	uint32_t edi;           // unused
	uint32_t es;            // unused
	uint32_t cs;            // unused
	uint32_t ss;            // unused
	uint32_t ds;            // unused
	uint32_t fs;            // unused
	uint32_t gs;            // unused
	uint32_t ldt;           // unused
	uint16_t trap;          // unused
	uint16_t iomap_base;    // unused
} __attribute__((packed)) tss_t;

void tss_set_stack(size_t esp0, size_t ss0);
void init_tss();

#endif