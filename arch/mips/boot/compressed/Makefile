#
# This file is subject to the terms and conditions of the GNU General Public
# License.
#
# Adapted for MIPS Pete Popov, Dan Malek
#
# Copyright (C) 1994 by Linus Torvalds
# Adapted for PowerPC by Gary Thomas
# modified by Cort (cort@cs.nmt.edu)
#
# Copyright (C) 2009 Lemote Inc. & DSLab, Lanzhou University
# Author: Wu Zhangjin <wuzhangjin@gmail.com>
#

GCOV_PROFILE := n

# Disable Function Tracer
KBUILD_CFLAGS := $(shell echo $(KBUILD_CFLAGS) | sed -e "s/-pg//")

KBUILD_CFLAGS := $(LINUXINCLUDE) $(KBUILD_CFLAGS)

KBUILD_AFLAGS := $(LINUXINCLUDE) $(KBUILD_AFLAGS) -D__ASSEMBLY__ \
	-DKERNEL_ENTRY=0x$(shell $(NM) $(objtree)/$(KBUILD_IMAGE) 2>/dev/null | grep " kernel_entry" | cut -f1 -d \ )

targets := head.o decompress.o cache.o dbg.o uart-16550.o uart-alchemy.o

# bootstrap code
vmlinuz-init  := $(obj)/head.o

# decompressor objects (linked with vmlinuz)
vmlinuzobjs-y := $(obj)/decompress.o $(obj)/cache.o

ifdef CONFIG_DEBUG_ZBOOT
vmlinuzobjs-y += $(obj)/dbg.o
vmlinuzobjs-$(CONFIG_SYS_SUPPORTS_ZBOOT_UART16550) += $(obj)/uart-16550.o
vmlinuzobjs-$(CONFIG_MACH_ALCHEMY)		   += $(obj)/uart-alchemy.o
endif

targets += vmlinux.bin
OBJCOPYFLAGS_vmlinux.bin := $(OBJCOPYFLAGS) -O binary -R .comment -S
$(obj)/vmlinux.bin: $(KBUILD_IMAGE) FORCE
	$(call if_changed,objcopy)

tool_$(CONFIG_KERNEL_GZIP)    = gzip
tool_$(CONFIG_KERNEL_BZIP2)   = bzip2
tool_$(CONFIG_KERNEL_LZMA)    = lzma
tool_$(CONFIG_KERNEL_LZO)     = lzo

targets += vmlinux.bin.z
$(obj)/vmlinux.bin.z: $(obj)/vmlinux.bin FORCE
	$(call if_changed,$(tool_y))

targets += piggy.o
vmlinuzobjs-y += $(obj)/piggy.o
OBJCOPYFLAGS_piggy.o := --add-section=.image=$(obj)/vmlinux.bin.z \
                        --set-section-flags=.image=contents,alloc,load,readonly,data
$(obj)/piggy.o: $(obj)/dummy.o $(obj)/vmlinux.bin.z FORCE
	$(call if_changed,objcopy)

# Calculate the load address of the compressed kernel image
VMLINUZ_LOAD_ADDRESS = $(shell $(objtree)/scripts/calc_vmlinuz_load_addr \
		$(obj)/vmlinux.bin $(VMLINUX_LOAD_ADDRESS))

KBUILD_CPPFLAGS += -D"VMLINUZ_LOAD_ADDRESS=$(VMLINUZ_LOAD_ADDRESS)"
CPPFLAGS_vmlinuz.lds += $(KBUILD_CFLAGS)
vmlinuz-lds := $(obj)/vmlinuz.lds

targets	+= vmlinuz.lds

LDFLAGS += --gc-sections -n

ifeq ($(KBUILD_VERBOSE),1)
LDFLAGS += --print-gc-sections
endif

quiet_cmd_zld = LD      $@
      cmd_zld = $(LD) $(LDFLAGS) -o $@ -T $< $(vmlinuz-init) \
		--start-group $(vmlinuzobjs-y) --end-group
quiet_cmd_strip = STRIP   $@
      cmd_strip = $(STRIP) -s $@
quiet_cmd_copy  = COPY    $< to $@
      cmd_copy  = cp $< $@
ifdef CONFIG_SSTRIP
quiet_cmd_sstrip = SSTRIP  $@
      cmd_sstrip = $(objtree)/scripts/sstrip $@
endif

vmlinuz-unsstrip := $(obj)/vmlinuz.unsstrip

vmlinuz: $(vmlinuz-unsstrip) FORCE
	$(call cmd,copy)
	$(call cmd,sstrip)

$(vmlinuz-unsstrip): $(vmlinuz-lds) $(vmlinuz-init) $(vmlinuzobjs-y) FORCE
	$(call cmd,zld)
	$(call cmd,strip)

#
# Some DECstations need all possible sections of an ECOFF executable
#
ifdef CONFIG_MACH_DECSTATION
  e2eflag := -a
endif

# elf2ecoff can only handle 32bit image
hostprogs-y := ../elf2ecoff

ifdef CONFIG_32BIT
	VMLINUZ = $(vmlinuz_unsstrip)
else
	VMLINUZ = vmlinuz.32
endif

quiet_cmd_32 = OBJCOPY $@
      cmd_32 = $(OBJCOPY) -O $(32bit-bfd) $(OBJCOPYFLAGS) $< $@
vmlinuz.32: $(vmlinuz_unsstrip)
	$(call cmd,32)

quiet_cmd_ecoff = ECOFF   $@
      cmd_ecoff = $< $(VMLINUZ) $@ $(e2eflag)
vmlinuz.ecoff: $(obj)/../elf2ecoff $(VMLINUZ)
	$(call cmd,ecoff)

OBJCOPYFLAGS_vmlinuz.bin := $(OBJCOPYFLAGS) -O binary
vmlinuz.bin: $(vmlinuz_unsstrip)
	$(call cmd,objcopy)

OBJCOPYFLAGS_vmlinuz.srec := $(OBJCOPYFLAGS) -S -O srec
vmlinuz.srec: $(vmlinuz_unsstrip)
	$(call cmd,objcopy)

clean-files := vmlinuz $(objtree)/vmlinuz.{32,ecoff,bin,srec,unsstrip,lds}
