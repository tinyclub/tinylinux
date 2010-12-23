#ifndef BOOT_COMPRESSED_BOOT_H
#define BOOT_COMPRESSED_BOOT_H

/* set the default size of the mallocing area for decompressing */
#ifdef CONFIG_KERNEL_BZIP2
#define BOOT_HEAP_SIZE 0x400000
#else
#define BOOT_HEAP_SIZE 0x8000
#endif

/* set the default stack size */
#ifdef CONFIG_64BIT
#define BOOT_STACK_SIZE 0x4000
#else
#define BOOT_STACK_SIZE 0x1000
#endif

#endif /* BOOT_COMPRESSED_BOOT_H */
