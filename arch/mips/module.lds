
/*
 * MIPS Specific module linker script, this will be merged to the
 * common linker script in scripts/module-common.lds
 */

SECTIONS {

	/DISCARD/ : {
		/* ABI crap starts here */
		*(.MIPS.options)
		*(.options)
		*(.pdr)
		*(.reginfo)
		*(.comment)
		*(.mdebug.*)
		*(.gnu.attributes)
	}
}
