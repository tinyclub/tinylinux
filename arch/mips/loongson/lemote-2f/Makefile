# Fail on warnings - also for files referenced in subdirs
# -Werror can be disabled for specific files using:
# CFLAGS_<file.o> := -Wno-error
subdir-ccflags-$(CONFIG_MIPS_WERROR) := -Werror

#
# Makefile for lemote loongson2f family machines
#

obj-y += irq.o reset.o ec_kb3310b.o

ifndef CONFIG_EMBEDDED
obj-y += machtype.o
endif

#
# Suspend Support
#

obj-$(CONFIG_LOONGSON_SUSPEND) += pm.o
