/*
 * Flush icache after overriding the original kernel image.
 */

#include <asm/addrspace.h>

#include "boot.h"

#define cache32_unroll32(base, op)					\
	__asm__ __volatile__(						\
	"	.set push					\n"	\
	"	.set noreorder					\n"	\
	"	.set mips3					\n"	\
	"	cache %1, 0x000(%0); cache %1, 0x020(%0)	\n"	\
	"	cache %1, 0x040(%0); cache %1, 0x060(%0)	\n"	\
	"	cache %1, 0x080(%0); cache %1, 0x0a0(%0)	\n"	\
	"	cache %1, 0x0c0(%0); cache %1, 0x0e0(%0)	\n"	\
	"	cache %1, 0x100(%0); cache %1, 0x120(%0)	\n"	\
	"	cache %1, 0x140(%0); cache %1, 0x160(%0)	\n"	\
	"	cache %1, 0x180(%0); cache %1, 0x1a0(%0)	\n"	\
	"	cache %1, 0x1c0(%0); cache %1, 0x1e0(%0)	\n"	\
	"	cache %1, 0x200(%0); cache %1, 0x220(%0)	\n"	\
	"	cache %1, 0x240(%0); cache %1, 0x260(%0)	\n"	\
	"	cache %1, 0x280(%0); cache %1, 0x2a0(%0)	\n"	\
	"	cache %1, 0x2c0(%0); cache %1, 0x2e0(%0)	\n"	\
	"	cache %1, 0x300(%0); cache %1, 0x320(%0)	\n"	\
	"	cache %1, 0x340(%0); cache %1, 0x360(%0)	\n"	\
	"	cache %1, 0x380(%0); cache %1, 0x3a0(%0)	\n"	\
	"	cache %1, 0x3c0(%0); cache %1, 0x3e0(%0)	\n"	\
	"	.set pop					\n"	\
		:							\
		: "r" (base),						\
		  "i" (op));

void flush_cache_all(void)
{
	unsigned long start = CKSEG0ADDR(0x80000000);
	unsigned long lsize = 32;
	unsigned long end;
	unsigned long addr;
	int i;

	/* Flush the low 64M, this should be enough for the biggest kernel image! */
	end = start + 64 * 1024 / 4;

	for (i = 0; i < 4; i++) {
		for (addr = start; addr < end; addr += lsize * 32)
			cache32_unroll32(addr | i, 0x03);	/* Index Writeback scache */
	}
}
