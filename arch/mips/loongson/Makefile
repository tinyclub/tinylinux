# Fail on warnings - also for files referenced in subdirs
# -Werror can be disabled for specific files using:
# CFLAGS_<file.o> := -Wno-error
subdir-ccflags-$(CONFIG_MIPS_WERROR) := -Werror

#
# Common code for all Loongson based systems
#

obj-$(CONFIG_MACH_LOONGSON) += common/

#
# Lemote Fuloong mini-PC (Loongson 2E-based)
#

obj-$(CONFIG_LEMOTE_FULOONG2E)  += fuloong-2e/

#
# Lemote loongson2f family machines
#

obj-$(CONFIG_LEMOTE_MACH2F)  += lemote-2f/
