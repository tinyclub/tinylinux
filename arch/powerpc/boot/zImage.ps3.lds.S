#include <asm/page.h>
#include <asm-generic/vmlinux.lds.h>
#include <asm/cache.h>

#undef powerpc

OUTPUT_ARCH(powerpc:common)
ENTRY(_zimage_start)
EXTERN(_zimage_start)
SECTIONS
{
  _vmlinux_start =  .;
  .kernel:vmlinux.bin : { KEEP(*(.kernel:vmlinux.bin)) }
  _vmlinux_end =  .;

  . = ALIGN(4096);
  _dtb_start = .;
  .kernel:dtb : { KEEP(*(.kernel:dtb)) }
  _dtb_end = .;

  . = ALIGN(4096);
  _initrd_start =  .;
  .kernel:initrd : { KEEP(*(.kernel:initrd)) }
  _initrd_end =  .;

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
    ALL(.sdata)
    __got2_start = .;
    *(.got2)
    __got2_end = .;
  }

  . = ALIGN(4096);
  _edata  =  .;

  . = ALIGN(4096);
  __bss_start = .;
  .bss       :
  {
   ALL(.sbss)
   ALL(.bss)
  }
  . = ALIGN(4096);
  _end = . ;
}
