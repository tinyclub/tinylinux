/*
 *  linux/arch/arm/kernel/ptrace.h
 *
 *  Copyright (C) 2000-2003 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/ptrace.h>

extern void ptrace_cancel_bpt(struct task_struct *);
extern void ptrace_set_bpt(struct task_struct *);
extern void ptrace_break(struct task_struct *, struct pt_regs *);

/*
 * Send SIGTRAP if we're single-stepping
 */
#ifdef CONFIG_PTRACE
static inline void single_step_trap(struct task_struct *task)
{
	if (task->ptrace & PT_SINGLESTEP) {
		ptrace_cancel_bpt(task);
		send_sig(SIGTRAP, task, 1);
	}
}
#else
#define single_step_trap(task) do { } while (0)
#endif

#ifdef CONFIG_PTRACE
static inline void single_step_clear(struct task_struct *task)
{
	if (task->ptrace & PT_SINGLESTEP)
		ptrace_cancel_bpt(task);
}
#else
#define single_step_clear(task) do { } while (0)
#endif

#ifdef CONFIG_PTRACE
static inline void single_step_set(struct task_struct *task)
{
	if (task->ptrace & PT_SINGLESTEP)
		ptrace_set_bpt(task);
}
#else
#define single_step_set(task) do { } while (0)
#endif
