SECTIONS {
	. = SIZEOF_HEADERS;
	.rodata : { *(.rodata) *(.rodata.*) } :ro
	.note : { *(.note*) }
	. = 0xa0000;
	.data : { *(.data) *(.data.*) } :dat
	/DISCARD/ : { *(*) }
}
PHDRS {
  ro PT_LOAD FILEHDR PHDRS;
  dat PT_LOAD;
}
