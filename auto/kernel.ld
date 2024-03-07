/* Buzz-OS kernel linker script ~ eylon */ 

/* Define output formats :: see available options with '$ objdump -i' */
OUTPUT_FORMAT(elf32-i386)       /* Specify the output file format :: equivalent to the --oformat flag */
OUTPUT_ARCH(i386)               /* Specify the machine architecture */

/* Set the start of the kernel */
ENTRY(_start)                              /* Define the kernel's entry point */
/*STARTUP(bin/kernel/kernel_entry.asm.o)  /* Put kernel_entry.asm.o at the start of the binary file */

/* Define output sections */
SECTIONS
{
    /* Define [_vstart] for [VMA] and [_pstart] for [LMA] */
    _vstart = 0xC0100000;
    _pstart = 0x100000;
    . = _vstart;


    /* <section .text> [VMA=.] [LMA=_pstart] */
    .text : AT(_pstart) {
        *(.text .text.* .stub .gnu.linkonce.t.*)
    }

    /* Define the [_etext] symbol to mark the end address of the text section */
    _etext = .;

	/* <section .rodata> [VMA=.] [LMA=.]*/
    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }

    /* Include debugging information in kernel's memory :: the defined symbols are used by the debugger to use the section */
    /* The .stab section is used to store debugging information in the kernel's memory */
	.stab : {       
		PROVIDE(__STAB_BEGIN__ = .);        /* Define a __STAB_BEGIN__ symbol to represent the start of the .stab section */
		*(.stab);                           /* Include all the debug data from the .stab sections in the input files */
		PROVIDE(__STAB_END__ = .);          /* Define a __STAB_END__ symbol to represent the end of the .stab section */
	}
    /* The .stabstr section is used to store string debug data like variables, functions and file names.*/
	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);     /* Define a __STABSTR_BEGIN__ symbol to represent the start of the .stabstr section */
		*(.stabstr);                        /* Include all the string debug data from the .stabstr sections in the input files */
		PROVIDE(__STABSTR_END__ = .);       /* Define a __STABSTR_END__ symbol to represent the end of the .stabstr section */
	}


    /* Align the address of the data segment to the next page */
	. = ALIGN(0x1000);

    /* Define the [_sdata] symbol to mark the start address of the data section */
	_sdata = .;

	/* <section .data> [VMA=.] [LMA=.]*/
	.data : {
		*(.data)
	}
    /* Define the 'edata' symbol to mark the end address of the data section */
	PROVIDE(edata = .);

    /* <section .bss> [VMA=.] [LMA=.] */
	.bss : {
		*(.bss)
	}

    /* Define an 'end' symbol to mark the end address of the linked output file */
	_vend = .;
    _pend = _pstart + (_vend - _vstart);


    /* Discard unused sections */
	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack)
	}
}

/*
    <<< Linker Custom Symbols Table >>>
- _vstart:      virtual address of the kernel base
- _pstart:      physical address of the kernel base
- _vend:        virtual address of the kernel top
- _pend:        physical address of the kernel top
- _etext:       virtual address of the end of the text section
- _sdata:       virtual address of the start of the data section

*/