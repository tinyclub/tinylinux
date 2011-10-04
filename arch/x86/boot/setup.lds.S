/*
 * Linker script for the i386 setup code
 */

#include <asm/page.h>
#include <asm-generic/vmlinux.lds.h>
#include <asm/cache.h>

#undef i386

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
	. = 0;
	.bstext		: { *(.bstext) }
	.bsdata		: { KEEP(*(.bsdata)) }

	. = 497;
	.header		: { *(.header) }
	.entrytext	: { *(.entrytext) }
	.inittext	: { *(.inittext) }
	.initdata	: { *(.initdata) }
	__end_init = .;

	.text		: { *(.text) KEEP(*(.text.*)) }
	.text32		: { *(.text32) }

	. = ALIGN(16);
	.rodata		: { ALL(.rodata) }

	.videocards	: {
		video_cards = .;
		*(.videocards)
		video_cards_end = .;
	}

	. = ALIGN(16);
	.data		: { ALL(.data) }

	.signature	: {
		setup_sig = .;
		LONG(0x5a5aaa55)
	}


	. = ALIGN(16);
	.bss		:
	{
		__bss_start = .;
		ALL(.bss)
		__bss_end = .;
	}
	. = ALIGN(16);
	_end = .;

	/DISCARD/ : { ALL(.note) }

	/*
	 * The ASSERT() sink to . is intentional, for binutils 2.14 compatibility:
	 */
	. = ASSERT(_end <= 0x8000, "Setup too big!");
	. = ASSERT(hdr == 0x1f1, "The setup header has the wrong offset!");
	/* Necessary for the very-old-loader check to work... */
	. = ASSERT(__end_init <= 5*512, "init sections too big!");

}
