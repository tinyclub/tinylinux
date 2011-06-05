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
