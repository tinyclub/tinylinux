/*
 *  Copyright (C) 1995, 1996, 2001  Ralf Baechle
 *  Copyright (C) 2001, 2004  MIPS Technologies, Inc.
 *  Copyright (C) 2004  Maciej W. Rozycki
 *  Copyright (C) 2010  Wu Zhangjin <wuzhangjin@gmail.com>
 */
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <asm/bootinfo.h>
#include <asm/cpu.h>
#include <asm/cpu-features.h>
#include <asm/mipsregs.h>
#include <asm/processor.h>

unsigned int vced_count, vcei_count;

static inline void show_full_cpuinfo(struct seq_file *m)
{
#ifdef CONFIG_SHOW_FULL_CPUINFO
	/* This can be copied to the processord specific cpu-feature-overrides.h */
	seq_printf(m, "# show_full_cpuinfo() start\n\n");
	seq_printf(m, "current_cpu_prid()\t0x%x\n", current_cpu_data.processor_id);
	seq_printf(m, "current_cpu_type()\t%d\n", current_cpu_data.cputype);
	seq_printf(m, "cpu_tlbsize()\t%d\n", cpu_data[0].tlbsize);
	seq_printf(m, "cpu_dcache_size()\t%ldUL\n", cpu_data[0].dcache.size);
	seq_printf(m, "cpu_icache_size()\t%ldUL\n", cpu_data[0].icache.size);
	seq_printf(m, "cpu_scache_size()\t%ldUL\n", cpu_data[0].scache.size);
	seq_printf(m, "cpu_dcache_sets()\t%d\n", cpu_data[0].dcache.sets);
	seq_printf(m, "cpu_icache_sets()\t%d\n", cpu_data[0].icache.sets);
	seq_printf(m, "cpu_scache_sets()\t%d\n", cpu_data[0].scache.sets);
	seq_printf(m, "cpu_dcache_line_size()\t%d\n", cpu_data[0].dcache.linesz);
	seq_printf(m, "cpu_icache_line_size()\t%d\n", cpu_data[0].icache.linesz);
	seq_printf(m, "cpu_scache_line_size()\t%d\n", cpu_data[0].scache.linesz);
	seq_printf(m, "cpu_dcache_way_size()\t%d\n", cpu_data[0].dcache.waysize);
	seq_printf(m, "cpu_icache_way_size()\t%d\n", cpu_data[0].icache.waysize);
	seq_printf(m, "cpu_scache_way_size()\t%d\n", cpu_data[0].scache.waysize);
	seq_printf(m, "cpu_dcache_waybit()\t%d\n", cpu_data[0].dcache.waybit);
	seq_printf(m, "cpu_icache_waybit()\t%d\n", cpu_data[0].icache.waybit);
	seq_printf(m, "cpu_scache_waybit()\t%d\n", cpu_data[0].scache.waybit);
	seq_printf(m, "cpu_dcache_ways()\t%d\n", cpu_data[0].dcache.ways);
	seq_printf(m, "cpu_icache_ways()\t%d\n", cpu_data[0].icache.ways);
	seq_printf(m, "cpu_scache_ways()\t%d\n", cpu_data[0].scache.ways);
	seq_printf(m, "cpu_dcache_flags()\t%d\n", cpu_data[0].dcache.flags);
	seq_printf(m, "cpu_icache_flags()\t%d\n", cpu_data[0].icache.flags);
	seq_printf(m, "cpu_scache_flags()\t%d\n", cpu_data[0].scache.flags);
#ifdef CONFIG_SMP
	seq_printf(m, "cpu_icache_snoops_remote_store\t%d\n", cpu_data[0].icache.flags & MIPS_IC_SNOOPS_REMOTE);
#else
	seq_printf(m, "cpu_icache_snoops_remote_store\t1\n");
#endif
	seq_printf(m, "cpu_watch_reg_count()\t%d\n", current_cpu_data.watch_reg_count);
	seq_printf(m, "cpu_watch_reg_use_cnt()\t%d\n", current_cpu_data.watch_reg_use_cnt);
	seq_printf(m, "cpu_isa_level()\t%d\n", current_cpu_data.isa_level);

	seq_printf(m, "cpu_srsets()\t%d\n", current_cpu_data.srsets);
	seq_printf(m, "cpu_vmbits\t%d\n", current_cpu_data.vmbits);
	seq_printf(m, "cpu_optoins()\t0x%lx\n", current_cpu_data.options);
	seq_printf(m, "cpu_fpu_id()\t0x%x\n", current_cpu_data.fpu_id);

	seq_printf(m, "cpu_has_32fpr\t%d\n", !!(cpu_data[0].options & MIPS_CPU_32FPR));
	seq_printf(m, "cpu_has_3k_cache\t%d\n", !!(cpu_data[0].options & MIPS_CPU_3K_CACHE));
	seq_printf(m, "cpu_has_4k_cache\t%d\n", !!(cpu_data[0].options & MIPS_CPU_4K_CACHE));
	seq_printf(m, "cpu_has_4kex\t%d\n", !!(cpu_data[0].options & MIPS_CPU_4KEX));
	seq_printf(m, "cpu_has_64bits\t%d\n", !!(cpu_data[0].isa_level & MIPS_CPU_ISA_64BIT));
	seq_printf(m, "cpu_has_64bit_zero_reg\t%d\n", !!(cpu_data[0].isa_level & MIPS_CPU_ISA_64BIT));
	seq_printf(m, "cpu_has_cache_cdex_p\t%d\n", !!(cpu_data[0].options & MIPS_CPU_CACHE_CDEX_P));
	seq_printf(m, "cpu_has_cache_cdex_s\t%d\n", !!(cpu_data[0].options & MIPS_CPU_CACHE_CDEX_S));
	seq_printf(m, "cpu_has_counter\t%d\n", !!(cpu_data[0].options & MIPS_CPU_COUNTER));
	seq_printf(m, "cpu_has_dc_aliases\t%d\n", !!(cpu_data[0].dcache.flags & MIPS_CACHE_ALIASES));
	seq_printf(m, "cpu_has_divec\t%d\n", !!(cpu_data[0].options & MIPS_CPU_DIVEC));
	seq_printf(m, "cpu_has_dsp\t%d\n", !!(cpu_data[0].ases & MIPS_ASE_DSP));
	seq_printf(m, "cpu_has_ejtag\t%d\n", !!(cpu_data[0].options & MIPS_CPU_EJTAG));
	seq_printf(m, "cpu_has_fpu\t%d\n", !!(current_cpu_data.options & MIPS_CPU_FPU));
	seq_printf(m, "cpu_has_ic_fills_f_dc\t%d\n", !!(cpu_data[0].icache.flags & MIPS_CACHE_IC_F_DC));
	seq_printf(m, "cpu_has_inclusive_pcaches\t%d\n", !!(cpu_data[0].options & MIPS_CPU_INCLUSIVE_CACHES));
	seq_printf(m, "cpu_has_llsc\t%d\n", !!(cpu_data[0].options & MIPS_CPU_LLSC));
	seq_printf(m, "cpu_has_mcheck\t%d\n", !!(cpu_data[0].options & MIPS_CPU_MCHECK));
	seq_printf(m, "cpu_has_mdmx\t%d\n", !!(cpu_data[0].ases & MIPS_ASE_MDMX));
	seq_printf(m, "cpu_has_mips16\t%d\n", !!(cpu_data[0].ases & MIPS_ASE_MIPS16));
	seq_printf(m, "cpu_has_mips32r1\t%d\n", !!(cpu_data[0].isa_level & MIPS_CPU_ISA_M32R1));
	seq_printf(m, "cpu_has_mips32r2\t%d\n", !!(cpu_data[0].isa_level & MIPS_CPU_ISA_M32R2));
	seq_printf(m, "cpu_has_mips3d\t%d\n", !!(cpu_data[0].ases & MIPS_ASE_MIPS3D));
	seq_printf(m, "cpu_has_mips64r1\t%d\n", !!(cpu_data[0].isa_level & MIPS_CPU_ISA_M64R1));
	seq_printf(m, "cpu_has_mips64r2\t%d\n", !!(cpu_data[0].isa_level & MIPS_CPU_ISA_M64R2));
	seq_printf(m, "cpu_has_mipsmt\t%d\n", !!(cpu_data[0].ases & MIPS_ASE_MIPSMT));
	seq_printf(m, "cpu_has_nofpuex\t%d\n", !!(cpu_data[0].options & MIPS_CPU_NOFPUEX));
	seq_printf(m, "cpu_has_pindexed_dcache\t%d\n", !!(cpu_data[0].dcache.flags & MIPS_CACHE_PINDEX));
	seq_printf(m, "cpu_has_prefetch\t%d\n", !!(cpu_data[0].options & MIPS_CPU_PREFETCH));
	seq_printf(m, "cpu_has_smartmips\t%d\n", !!(cpu_data[0].ases & MIPS_ASE_SMARTMIPS));
	seq_printf(m, "cpu_has_tlb\t%d\n", !!(cpu_data[0].options & MIPS_CPU_TLB));
	seq_printf(m, "cpu_has_tx39_cache\t%d\n", !!(cpu_data[0].options & MIPS_CPU_TX39_CACHE));
	seq_printf(m, "cpu_has_userlocal\t%d\n", !!(cpu_data[0].options & MIPS_CPU_ULRI));
	seq_printf(m, "cpu_has_vce\t%d\n", !!(cpu_data[0].options & MIPS_CPU_VCE));
	seq_printf(m, "cpu_has_veic\t%d\n", !!(cpu_data[0].options & MIPS_CPU_VEIC));
	seq_printf(m, "cpu_has_vint\t%d\n", !!(cpu_data[0].options & MIPS_CPU_VINT));
	seq_printf(m, "cpu_has_vtag_icache\t%d\n", !!(cpu_data[0].icache.flags & MIPS_CACHE_VTAG));
	seq_printf(m, "cpu_has_watch\t%d\n", !!(cpu_data[0].options & MIPS_CPU_WATCH));
	seq_printf(m, "\n\n# show_full_cpuinfo() end\n");
#endif /* CONFIG_SHOW_FULL_CPUINFO */
}

static int show_cpuinfo(struct seq_file *m, void *v)
{
	unsigned long n = (unsigned long) v - 1;
	unsigned int version = cpu_data[n].processor_id;
	unsigned int fp_vers = cpu_data[n].fpu_id;
	char fmt [64];
	int i;

#ifdef CONFIG_SMP
	if (!cpu_isset(n, cpu_online_map))
		return 0;
#endif

	show_full_cpuinfo(m);

	/*
	 * For the first processor also print the system type
	 */
	if (n == 0)
		seq_printf(m, "system type\t\t: %s\n", get_system_type());

	seq_printf(m, "processor\t\t: %ld\n", n);
	sprintf(fmt, "cpu model\t\t: %%s V%%d.%%d%s\n",
	        cpu_data[n].options & MIPS_CPU_FPU ? "  FPU V%d.%d" : "");
	seq_printf(m, fmt, __cpu_name[n],
	                           (version >> 4) & 0x0f, version & 0x0f,
	                           (fp_vers >> 4) & 0x0f, fp_vers & 0x0f);
	seq_printf(m, "BogoMIPS\t\t: %u.%02u\n",
	              cpu_data[n].udelay_val / (500000/HZ),
	              (cpu_data[n].udelay_val / (5000/HZ)) % 100);
	seq_printf(m, "wait instruction\t: %s\n", cpu_wait ? "yes" : "no");
	seq_printf(m, "microsecond timers\t: %s\n",
	              cpu_has_counter ? "yes" : "no");
	seq_printf(m, "tlb_entries\t\t: %d\n", cpu_data[n].tlbsize);
	seq_printf(m, "extra interrupt vector\t: %s\n",
	              cpu_has_divec ? "yes" : "no");
	seq_printf(m, "hardware watchpoint\t: %s",
		   cpu_has_watch ? "yes, " : "no\n");
	if (cpu_has_watch) {
		seq_printf(m, "count: %d, address/irw mask: [",
			   cpu_data[n].watch_reg_count);
		for (i = 0; i < cpu_data[n].watch_reg_count; i++)
			seq_printf(m, "%s0x%04x", i ? ", " : "" ,
				   cpu_data[n].watch_reg_masks[i]);
		seq_printf(m, "]\n");
	}
	seq_printf(m, "ASEs implemented\t:%s%s%s%s%s%s\n",
		      cpu_has_mips16 ? " mips16" : "",
		      cpu_has_mdmx ? " mdmx" : "",
		      cpu_has_mips3d ? " mips3d" : "",
		      cpu_has_smartmips ? " smartmips" : "",
		      cpu_has_dsp ? " dsp" : "",
		      cpu_has_mipsmt ? " mt" : ""
		);
	seq_printf(m, "shadow register sets\t: %d\n",
		       cpu_data[n].srsets);
	seq_printf(m, "core\t\t\t: %d\n", cpu_data[n].core);

	sprintf(fmt, "VCE%%c exceptions\t\t: %s\n",
	        cpu_has_vce ? "%u" : "not available");
	seq_printf(m, fmt, 'D', vced_count);
	seq_printf(m, fmt, 'I', vcei_count);
	seq_printf(m, "\n");

	return 0;
}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	unsigned long i = *pos;

	return i < NR_CPUS ? (void *) (i + 1) : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return c_start(m, pos);
}

static void c_stop(struct seq_file *m, void *v)
{
}

const struct seq_operations cpuinfo_op = {
	.start	= c_start,
	.next	= c_next,
	.stop	= c_stop,
	.show	= show_cpuinfo,
};
