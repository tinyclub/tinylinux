#include <asm/page.h>
#include <asm-generic/vmlinux.lds.h>
#include <asm/cache.h>

#undef powerpc

OUTPUT_ARCH(powerpc:common)
ENTRY(_zimage_start_opd)
EXTERN(_zimage_start_opd)
SECTIONS
{
  _start = .;
  .text      :
  {
    ALL(.text)
    *(.fixup)
  }
  _etext = .;
  . = ALIGN(4096);
  .data    :
  {
    ALL(.rodata)
    ALL(.data)
    *(__builtin_*)
    ALL(.sdata)
    __got2_start = .;
    *(.got2)
    __got2_end = .;

    _dtb_start = .;
    KEEP(*(.kernel:dtb))
    _dtb_end = .;

    _vmlinux_start =  .;
    KEEP(*(.kernel:vmlinux.strip))
    _vmlinux_end =  .;

    _initrd_start =  .;
    KEEP(*(.kernel:initrd))
    _initrd_end =  .;
  }

  . = ALIGN(4096);
  _edata  =  .;
  __bss_start = .;
  .bss       :
  {
   ALL(.sbss)
   ALL(.bss)
  }
  _end = . ;

  /DISCARD/ :
  {
    *(.comment)
  }
}
