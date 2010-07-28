SECTIONS
{
        .text :
        {
        __stext = . ;
	__text = .;
	       *(.text..startup)
	       *(.text)
	       *(.text.*)
        __etext = . ;
        }

	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
	}
        .data :

        {
        __sdata = . ;
        ___data_start = . ;
                *(.data.*)
	}
        .bss :
        {
        . = ALIGN(0x4) ;
        __sbss = . ;
                *(.bss*)
        . = ALIGN(0x4) ;
        __ebss = . ;
        __end = . ;
        }
}
