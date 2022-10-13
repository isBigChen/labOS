/* Simple linker script for the testos kernel.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS {
    . = 0x8200;

    .text : {
        *(.text.main);
        *(.text*);
    }
	
	PROVIDE(etext = .);

	.rodata : {
		*(.rodata .rodata.*)
	}

	/* Adjust the address for the data segment to the next page */
	. = ALIGN(0x1000);

	.data : {
		*(.data)
	}

	PROVIDE(edata = .);

	.bss : {
		*(.bss)
	}

	PROVIDE(end = .);
    
    /DISCARD/ : {
        *(.eh_frame .note.GNU-stack .comment)
    }
}
