/*
 * ld.script for compressed kernel support of MIPS
 *
 * Copyright (C) 2009 Lemote Inc.
 * Author: Wu Zhangjin <wuzhanjing@gmail.com>
 * Copyright (C) 2010 "Wu Zhangjin" <wuzhanjing@gmail.com>
 */

#include <asm/page.h>
#include <asm-generic/vmlinux.lds.h>
#include <asm/cache.h>

OUTPUT_ARCH(mips)
ENTRY(start)
SECTIONS
{
	/* Text and read-only data */
	/* . = VMLINUZ_LOAD_ADDRESS; */
	.text : {
		ALL(.text.start)
		/* Put the compressed image here */
		__image_begin = .;
		KEEP(*(.image))
		__image_end = .;
		ALL(.text)
		ALL(.rodata)
	}

	/* BSS */
	.bss : {
		__bss_begin = .;
		ALL(.bss)
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
