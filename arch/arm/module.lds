/*
 * ARM Specific module linker script, this will be merged to the
 * common linker script in scripts/module-common.lds
 */

SECTIONS {
	/DISCARD/ : {
		*(.ARM.exidx.exit.text)
		*(.ARM.extab.exit.text)
#ifndef CONFIG_HOTPLUG_CPU
		*(.ARM.exidx.cpuexit.text)
		*(.ARM.extab.cpuexit.text)
#endif
#ifndef CONFIG_HOTPLUG
		*(.ARM.exidx.devexit.text)
		*(.ARM.extab.devexit.text)
#endif
#ifndef CONFIG_MMU
		*(.fixup)
		*(__ex_table)
#endif
	}
}
