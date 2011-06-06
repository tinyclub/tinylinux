/*
 * Copyright (C) 2009 Lemote, Inc.
 * Author: Wu Zhangjin <wuzhangjin@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_MACH_LOONGSON_MACHINE_H
#define __ASM_MACH_LOONGSON_MACHINE_H

#ifdef CONFIG_LEMOTE_FULOONG2E

#define LOONGSON_MACHTYPE MACH_LEMOTE_FL2E

#ifdef CONFIG_EMBEDDED
#define mips_machtype MACH_LEMOTE_FL2E
#endif

#endif

/* use fuloong2f as the default machine of LEMOTE_MACH2F */
#ifdef CONFIG_LEMOTE_MACH2F

#define LOONGSON_MACHTYPE MACH_LEMOTE_FL2F

#ifdef CONFIG_EMBEDDED

#ifdef CONFIG_LEMOTE_YEELOONG
#define mips_machtype MACH_LEMOTE_YL2F89
#endif

#ifdef CONFIG_LEMOTE_FULOONG2F
#define mips_machtype MACH_LEMOTE_FL2F
#endif

#ifdef CONFIG_LEMOTE_LYNLOONG
#define mips_machtype MACH_LEMOTE_LL2F
#endif

#ifdef CONFIG_LEMOTE_UNAS
#define mips_machtype MACH_LEMOTE_NAS
#endif

#ifdef CONFIG_LEMOTE_MENGLOONG
#define mips_machtype MACH_LEMOTE_ML2F7
#endif

#endif /* CONFIG_EMBEDDED */

#endif

#endif /* __ASM_MACH_LOONGSON_MACHINE_H */
