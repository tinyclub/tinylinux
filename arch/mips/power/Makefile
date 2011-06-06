# Fail on warnings - also for files referenced in subdirs
# -Werror can be disabled for specific files using:
# CFLAGS_<file.o> := -Wno-error
subdir-ccflags-$(CONFIG_MIPS_WERROR) := -Werror

obj-$(CONFIG_HIBERNATION) += cpu.o hibernate.o
