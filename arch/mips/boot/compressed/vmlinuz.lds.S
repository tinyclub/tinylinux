/*
 * ld.script for compressed kernel support of MIPS
 *
 * Copyright (C) 2009 Lemote Inc.
 * Author: Wu Zhangjin <wuzhanjing@gmail.com>
 * Copyright (C) 2010 "Wu Zhangjin" <wuzhanjing@gmail.com>
 */

OUTPUT_ARCH(mips)
ENTRY(__start)
SECTIONS
{
	/* Text and read-only data */
	. = VMLINUZ_LOAD_ADDRESS;
	.text : {
		*(.text.asm.start)
		*(.text.*)
		*(.text.[A-Za-z0-9_$^]*)
		*(.rodata)
		*(.rodata.*)
		*(.rodata.[A-Za-z0-9_$^]*)
		/* Put the compressed image here */
		__image_begin = .;
		KEEP(*(.image))
		__image_end = .;
	}

	/* BSS */
	.bss : {
		__bss_begin = .;
		*(.bss)
		*(.bss.*)
		*(.bss.[A-Za-z0-9_$^]*)
		__bss_end = .;
	}

	/* Sections to be discarded */
	/DISCARD/ : {
		*(.MIPS.options)
		*(.options)
		*(.pdr)
		*(.reginfo)
		*(.comment)
		*(.note)
		*(.gnu.attributes)
		*(.data)
	}
}
