/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003, 2004 Ralf Baechle
 * Copyright (C) 2004  Maciej W. Rozycki
 */
#ifndef __ASM_CPU_FEATURES_H
#define __ASM_CPU_FEATURES_H

#include <asm/cpu.h>
#include <asm/cpu-info.h>
#include <cpu-feature-overrides.h>

#ifndef current_cpu_prid
#define current_cpu_prid()	current_cpu_data.processor_id
#endif

#ifndef current_cpu_type
#define current_cpu_type()	current_cpu_data.cputype
#endif

#ifndef cpu_options
#define cpu_options()	cpu_data[0].options
#endif

/*
 * SMP assumption: Options of CPU 0 are a superset of all processors.
 * This is true for all known MIPS systems.
 */

#ifndef cpu_tlbsize
#define cpu_tlbsize()		current_cpu_data.tlbsize	
#endif

#ifndef cpu_has_tlb
#define cpu_has_tlb		(cpu_options() & MIPS_CPU_TLB)
#endif
#ifndef cpu_has_4kex
#define cpu_has_4kex		(cpu_options() & MIPS_CPU_4KEX)
#endif
#ifndef cpu_has_3k_cache
#define cpu_has_3k_cache	(cpu_options() & MIPS_CPU_3K_CACHE)
#endif
#define cpu_has_6k_cache	0
#define cpu_has_8k_cache	0
#ifndef cpu_has_4k_cache
#define cpu_has_4k_cache	(cpu_options() & MIPS_CPU_4K_CACHE)
#endif
#ifndef cpu_has_tx39_cache
#define cpu_has_tx39_cache	(cpu_options() & MIPS_CPU_TX39_CACHE)
#endif
#ifndef cpu_has_octeon_cache
#define cpu_has_octeon_cache	0
#endif
#ifndef cpu_has_fpu
#define cpu_has_fpu		(current_cpu_data.options & MIPS_CPU_FPU)
#define raw_cpu_has_fpu		(raw_current_cpu_data.options & MIPS_CPU_FPU)
#else
#define raw_cpu_has_fpu		cpu_has_fpu
#endif

#ifndef cpu_fpu_id
#define cpu_fpu_id()		current_cpu_data.fpu_id
#endif

#ifndef cpu_has_32fpr
#define cpu_has_32fpr		(cpu_options() & MIPS_CPU_32FPR)
#endif
#ifndef cpu_has_counter
#define cpu_has_counter		(cpu_options() & MIPS_CPU_COUNTER)
#endif
#ifndef cpu_has_watch
#define cpu_has_watch		(cpu_options() & MIPS_CPU_WATCH)
#endif

#ifndef cpu_watch_reg_count
#define cpu_watch_reg_count()	current_cpu_data.watch_reg_count	
#endif

#ifndef cpu_watch_reg_use_cnt
#define cpu_watch_reg_use_cnt()	current_cpu_data.watch_reg_use_cnt
#endif

#ifndef cpu_srsets
#define cpu_srsets()		current_cpu_data.srsets
#endif

#ifndef cpu_has_divec
#define cpu_has_divec		(cpu_options() & MIPS_CPU_DIVEC)
#endif
#ifndef cpu_has_vce
#define cpu_has_vce		(cpu_options() & MIPS_CPU_VCE)
#endif
#ifndef cpu_has_cache_cdex_p
#define cpu_has_cache_cdex_p	(cpu_options() & MIPS_CPU_CACHE_CDEX_P)
#endif
#ifndef cpu_has_cache_cdex_s
#define cpu_has_cache_cdex_s	(cpu_options() & MIPS_CPU_CACHE_CDEX_S)
#endif
#ifndef cpu_has_prefetch
#define cpu_has_prefetch	(cpu_options() & MIPS_CPU_PREFETCH)
#endif
#ifndef cpu_has_mcheck
#define cpu_has_mcheck		(cpu_options() & MIPS_CPU_MCHECK)
#endif
#ifndef cpu_has_ejtag
#define cpu_has_ejtag		(cpu_options() & MIPS_CPU_EJTAG)
#endif
#ifndef cpu_has_llsc
#define cpu_has_llsc		(cpu_options() & MIPS_CPU_LLSC)
#endif
#ifndef kernel_uses_llsc
#define kernel_uses_llsc	cpu_has_llsc
#endif
#ifndef cpu_has_mips16
#define cpu_has_mips16		(cpu_data[0].ases & MIPS_ASE_MIPS16)
#endif
#ifndef cpu_has_mdmx
#define cpu_has_mdmx           (cpu_data[0].ases & MIPS_ASE_MDMX)
#endif
#ifndef cpu_has_mips3d
#define cpu_has_mips3d         (cpu_data[0].ases & MIPS_ASE_MIPS3D)
#endif
#ifndef cpu_has_smartmips
#define cpu_has_smartmips      (cpu_data[0].ases & MIPS_ASE_SMARTMIPS)
#endif
#ifndef kernel_uses_smartmips_rixi
#define kernel_uses_smartmips_rixi 0
#endif
#ifndef cpu_has_vtag_icache
#define cpu_has_vtag_icache	(cpu_data[0].icache.flags & MIPS_CACHE_VTAG)
#endif
#ifndef cpu_has_dc_aliases
#define cpu_has_dc_aliases	(cpu_data[0].dcache.flags & MIPS_CACHE_ALIASES)
#endif
#ifndef cpu_has_ic_fills_f_dc
#define cpu_has_ic_fills_f_dc	(cpu_data[0].icache.flags & MIPS_CACHE_IC_F_DC)
#endif
#ifndef cpu_has_pindexed_dcache
#define cpu_has_pindexed_dcache	(cpu_data[0].dcache.flags & MIPS_CACHE_PINDEX)
#endif

/*
 * I-Cache snoops remote store.  This only matters on SMP.  Some multiprocessors
 * such as the R10000 have I-Caches that snoop local stores; the embedded ones
 * don't.  For maintaining I-cache coherency this means we need to flush the
 * D-cache all the way back to whever the I-cache does refills from, so the
 * I-cache has a chance to see the new data at all.  Then we have to flush the
 * I-cache also.
 * Note we may have been rescheduled and may no longer be running on the CPU
 * that did the store so we can't optimize this into only doing the flush on
 * the local CPU.
 */
#ifndef cpu_icache_snoops_remote_store
#ifdef CONFIG_SMP
#define cpu_icache_snoops_remote_store	(cpu_data[0].icache.flags & MIPS_IC_SNOOPS_REMOTE)
#else
#define cpu_icache_snoops_remote_store	1
#endif
#endif

#ifndef cpu_isa_level
#define cpu_isa_level()		cpu_data[0].isa_level
#endif

# ifndef cpu_has_mips32r1
# define cpu_has_mips32r1	(cpu_isa_level() & MIPS_CPU_ISA_M32R1)
# endif
# ifndef cpu_has_mips32r2
# define cpu_has_mips32r2	(cpu_isa_level() & MIPS_CPU_ISA_M32R2)
# endif
# ifndef cpu_has_mips64r1
# define cpu_has_mips64r1	(cpu_isa_level() & MIPS_CPU_ISA_M64R1)
# endif
# ifndef cpu_has_mips64r2
# define cpu_has_mips64r2	(cpu_isa_level() & MIPS_CPU_ISA_M64R2)
# endif

/*
 * Shortcuts ...
 */
#define cpu_has_mips32	(cpu_has_mips32r1 | cpu_has_mips32r2)
#define cpu_has_mips64	(cpu_has_mips64r1 | cpu_has_mips64r2)
#define cpu_has_mips_r1	(cpu_has_mips32r1 | cpu_has_mips64r1)
#define cpu_has_mips_r2	(cpu_has_mips32r2 | cpu_has_mips64r2)
#define cpu_has_mips_r	(cpu_has_mips32r1 | cpu_has_mips32r2 | \
			 cpu_has_mips64r1 | cpu_has_mips64r2)

#ifndef cpu_has_mips_r2_exec_hazard
#define cpu_has_mips_r2_exec_hazard cpu_has_mips_r2
#endif

/*
 * MIPS32, MIPS64, VR5500, IDT32332, IDT32334 and maybe a few other
 * pre-MIPS32/MIPS53 processors have CLO, CLZ.  For 64-bit kernels
 * cpu_has_clo_clz also indicates the availability of DCLO and DCLZ.
 */
# ifndef cpu_has_clo_clz
# define cpu_has_clo_clz	cpu_has_mips_r
# endif

#ifndef cpu_has_dsp
#define cpu_has_dsp		(cpu_data[0].ases & MIPS_ASE_DSP)
#endif

#ifndef cpu_has_mipsmt
#define cpu_has_mipsmt		(cpu_data[0].ases & MIPS_ASE_MIPSMT)
#endif

#ifndef cpu_has_userlocal
#define cpu_has_userlocal	(cpu_options() & MIPS_CPU_ULRI)
#endif

#ifdef CONFIG_32BIT
# ifndef cpu_has_nofpuex
# define cpu_has_nofpuex	(cpu_options() & MIPS_CPU_NOFPUEX)
# endif
# ifndef cpu_has_64bits
# define cpu_has_64bits		(cpu_isa_level() & MIPS_CPU_ISA_64BIT)
# endif
# ifndef cpu_has_64bit_zero_reg
# define cpu_has_64bit_zero_reg	(cpu_isa_level() & MIPS_CPU_ISA_64BIT)
# endif
# ifndef cpu_has_64bit_gp_regs
# define cpu_has_64bit_gp_regs		0
# endif
# ifndef cpu_has_64bit_addresses
# define cpu_has_64bit_addresses	0
# endif
# ifndef cpu_vmbits
# define cpu_vmbits 31
# endif
#endif

#ifdef CONFIG_64BIT
# ifndef cpu_has_nofpuex
# define cpu_has_nofpuex		0
# endif
# ifndef cpu_has_64bits
# define cpu_has_64bits			1
# endif
# ifndef cpu_has_64bit_zero_reg
# define cpu_has_64bit_zero_reg		1
# endif
# ifndef cpu_has_64bit_gp_regs
# define cpu_has_64bit_gp_regs		1
# endif
# ifndef cpu_has_64bit_addresses
# define cpu_has_64bit_addresses	1
# endif
# ifndef cpu_vmbits
# define cpu_vmbits cpu_data[0].vmbits
# define __NEED_VMBITS_PROBE
# endif
#endif

#if defined(CONFIG_CPU_MIPSR2_IRQ_VI) && !defined(cpu_has_vint)
# define cpu_has_vint		(cpu_options() & MIPS_CPU_VINT)
#elif !defined(cpu_has_vint)
# define cpu_has_vint			0
#endif

#if defined(CONFIG_CPU_MIPSR2_IRQ_EI) && !defined(cpu_has_veic)
# define cpu_has_veic		(cpu_options() & MIPS_CPU_VEIC)
#elif !defined(cpu_has_veic)
# define cpu_has_veic			0
#endif

#ifndef cpu_has_inclusive_pcaches
#define cpu_has_inclusive_pcaches	(cpu_options() & MIPS_CPU_INCLUSIVE_CACHES)
#endif

#ifndef cpu_dcache_flags
#define cpu_dcache_flags()	cpu_data[0].dcache.flags
#endif
#ifndef cpu_icache_flags
#define cpu_icache_flags()	cpu_data[0].icache.flags
#endif
#ifndef cpu_scache_flags
#define cpu_scache_flags()	cpu_data[0].scache.flags
#endif

#ifndef cpu_dcache_ways
#define cpu_dcache_ways()	cpu_data[0].dcache.ways
#endif
#ifndef cpu_icache_ways
#define cpu_icache_ways()	cpu_data[0].icache.ways
#endif
#ifndef cpu_scache_ways
#define cpu_scache_ways()	cpu_data[0].scache.ways
#endif

#ifndef cpu_dcache_waybit
#define cpu_dcache_waybit()	cpu_data[0].dcache.waybit
#endif
#ifndef cpu_icache_waybit
#define cpu_icache_waybit()	cpu_data[0].icache.waybit
#endif
#ifndef cpu_scache_waybit
#define cpu_scache_waybit()	cpu_data[0].scache.waybit
#endif

#ifndef cpu_dcache_way_size
#define cpu_dcache_way_size()	cpu_data[0].dcache.waysize
#endif
#ifndef cpu_icache_way_size
#define cpu_icache_way_size()	cpu_data[0].icache.waysize
#endif
#ifndef cpu_scache_way_size
#define cpu_scache_way_size()	cpu_data[0].scache.waysize
#endif

#ifndef cpu_dcache_size
#define cpu_dcache_size()	cpu_data[0].dcache.size
#endif
#ifndef cpu_icache_size
#define cpu_icache_size()	cpu_data[0].icache.size
#endif
#ifndef cpu_scache_size
#define cpu_scache_size()	cpu_data[0].scache.size
#endif

#ifndef cpu_dcache_sets
#define cpu_dcache_sets()	cpu_data[0].dcache.sets
#endif
#ifndef cpu_icache_sets
#define cpu_icache_sets()	cpu_data[0].icache.sets
#endif
#ifndef cpu_scache_sets
#define cpu_scache_sets()	cpu_data[0].scache.sets
#endif

#ifndef cpu_dcache_line_size
#define cpu_dcache_line_size()	cpu_data[0].dcache.linesz
#endif
#ifndef cpu_icache_line_size
#define cpu_icache_line_size()	cpu_data[0].icache.linesz
#endif
#ifndef cpu_scache_line_size
#define cpu_scache_line_size()	cpu_data[0].scache.linesz
#endif

#ifndef cpu_hwrena_impl_bits
#define cpu_hwrena_impl_bits		0
#endif

#endif /* __ASM_CPU_FEATURES_H */
