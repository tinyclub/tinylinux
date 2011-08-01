/*
 * X86 Specific module linker script, this will be merged to the
 * common linker script in scripts/module-common.lds
 */

SECTIONS {
	/DISCARD/ : {
		*(.eh_frame)
	}
}
