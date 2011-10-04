#ifndef _ASM_X86_VSYSCALL_H
#define _ASM_X86_VSYSCALL_H

enum vsyscall_num {
	__NR_vgettimeofday,
	__NR_vtime,
	__NR_vgetcpu,
};

#define VSYSCALL_START (-10UL << 20)
#define VSYSCALL_SIZE 1024
#define VSYSCALL_END (-2UL << 20)
#define VSYSCALL_MAPPED_PAGES 1
#define VSYSCALL_ADDR(vsyscall_nr) (VSYSCALL_START+VSYSCALL_SIZE*(vsyscall_nr))

#ifdef __KERNEL__
#include <linux/seqlock.h>

#define __section_vgetcpu_mode __section_unused_aligned(.vgetcpu_mode, 16)
#define __section_jiffies __section_aligned(.jiffies, 16)

/* Definitions for CONFIG_GENERIC_TIME definitions */
#define __section_vsyscall_gtod_data __section_unused_aligned(.vsyscall_gtod_data, 16)
#define __section_vsyscall_clock __section_unused_aligned(.vsyscall_clock, 16)
#define __vsyscall_fn __section_unused(.vsyscall_fn) notrace

#define VGETCPU_RDTSCP	1
#define VGETCPU_LSL	2

extern int __vgetcpu_mode;
extern volatile unsigned long __jiffies;

/* kernel space (writeable) */
extern int vgetcpu_mode;
extern struct timezone sys_tz;

extern void map_vsyscall(void);

#endif /* __KERNEL__ */

#endif /* _ASM_X86_VSYSCALL_H */
