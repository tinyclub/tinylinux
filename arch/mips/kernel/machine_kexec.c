/*
 * machine_kexec.c for kexec
 * Created by <nschichan@corp.free.fr> on Thu Oct 12 15:15:06 2006
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2.  See the file COPYING for more details.
 */

#include <linux/kexec.h>
#include <linux/mm.h>
#include <linux/delay.h>

#include <asm/bootinfo.h>
#include <asm/cacheflush.h>
#include <asm/page.h>

int (*_machine_kexec_prepare)(struct kimage *) = NULL;
void (*_machine_kexec_shutdown)(void) = NULL;
void (*_machine_crash_shutdown)(struct pt_regs *regs) = NULL;
#ifdef CONFIG_SMP
void (*relocated_kexec_smp_wait) (void *);
atomic_t kexec_ready_to_reboot = ATOMIC_INIT(0);
#endif

static void machine_kexec_init_args(void)
{
	kexec_args[0] = fw_arg0;
	kexec_args[1] = fw_arg1;
	kexec_args[2] = fw_arg2;
	kexec_args[3] = fw_arg3;

	pr_info("kexec_args[0] (argc): %lu\n", kexec_args[0]);
	pr_info("kexec_args[1] (argv): %p\n", (void *)kexec_args[1]);
	pr_info("kexec_args[2] (env ): %p\n", (void *)kexec_args[2]);
	pr_info("kexec_args[3] (desc): %p\n", (void *)kexec_args[3]);
}

int
machine_kexec_prepare(struct kimage *kimage)
{
	/*
	 * Whenever arguments passed from kexec-tools, Init the arguments as
	 * the original ones to avoid booting failure.
	 *
	 * This can be overrided by _machine_kexec_prepare().
	 */
	machine_kexec_init_args();

	if (_machine_kexec_prepare)
		return _machine_kexec_prepare(kimage);
	return 0;
}

void
machine_kexec_cleanup(struct kimage *kimage)
{
}

void
machine_shutdown(void)
{
	if (_machine_kexec_shutdown)
		_machine_kexec_shutdown();
}

void
machine_crash_shutdown(struct pt_regs *regs)
{
	if (_machine_crash_shutdown)
		_machine_crash_shutdown(regs);
	else
		default_machine_crash_shutdown(regs);
}

typedef void (*noretfun_t)(void) __attribute__((noreturn));

void
machine_kexec(struct kimage *image)
{
	unsigned long kexec_relocate_size;
	unsigned long reboot_code_buffer;
	unsigned long entry;
	unsigned long *ptr;

	kexec_relocate_size = (unsigned long)(&__end___kexec_relocate) -
		(unsigned long)(&__start___kexec_relocate);
	pr_info("kexec_relocate_size = %lu\n", kexec_relocate_size);

	reboot_code_buffer =
	  (unsigned long)page_address(image->control_code_page);
	pr_info("reboot_code_buffer = %p\n", (void *)reboot_code_buffer);

	kexec_start_address =
		(unsigned long) phys_to_virt(image->start);
	pr_info("kexec_start_address(entry point of new kernel) = %p\n",
			(void *)kexec_start_address);

	kexec_indirection_page =
		(unsigned long) phys_to_virt(image->head & PAGE_MASK);
	pr_info("kexec_indirection_page = %p\n",
			(void *)kexec_indirection_page);

	memcpy((void *)reboot_code_buffer, &__start___kexec_relocate,
	       kexec_relocate_size);

	pr_info("Copy kexec_relocate section from %p to reboot_code_buffer: %p\n",
			&__start___kexec_relocate, (void *)reboot_code_buffer);

	/*
	 * The generic kexec code builds a page list with physical
	 * addresses. they are directly accessible through KSEG0 (or
	 * CKSEG0 or XPHYS if on 64bit system), hence the
	 * phys_to_virt() call.
	 */
	for (ptr = &image->head; (entry = *ptr) && !(entry &IND_DONE);
	     ptr = (entry & IND_INDIRECTION) ?
	       phys_to_virt(entry & PAGE_MASK) : ptr + 1) {
		if (*ptr & IND_SOURCE || *ptr & IND_INDIRECTION ||
		    *ptr & IND_DESTINATION)
			*ptr = (unsigned long) phys_to_virt(*ptr);
	}

	/*
	 * we do not want to be bothered.
	 */
	local_irq_disable();

	pr_info("Will call new kernel at %p\n", (void *)kexec_start_address);
	pr_info("Bye ...\n");
	__flush_cache_all();
#ifdef CONFIG_SMP
	/* All secondary cpus now may jump to kexec_wait cycle */
	relocated_kexec_smp_wait = reboot_code_buffer +
		(void *)(kexec_smp_wait - relocate_new_kernel);
	smp_wmb();
	atomic_set(&kexec_ready_to_reboot, 1);
#endif
	((noretfun_t) reboot_code_buffer)();
}
