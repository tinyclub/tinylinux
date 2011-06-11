#!/bin/bash
# author: falcon <wuzhangjin@gmail.com>
# update: Sat Jun 11 15:04:49 CST 2011

# Config
arch=mips

# Unused system calls
# System calls defined in kernel/sys_ni.c

unused_scalls="$(nm vmlinux | grep "W" | egrep " compat_|sys_|sys32_" | cut -d' ' -f3)"

# System call file list

case $arch in
	mips)
		scall_flist="$(ls arch/mips/kernel/scall*.o | sed -e 's/.o$/.S/g')"
		;;
	*)
		echo "Please fill the arch specific system call file list here."
		echo "They should be under arch/$arch/kernel/"
		;;
esac

for scall_file in $scall_flist
do
	for scall in $unused_scalls
	do
		sed -i -e "s/$scall\([[:space:]]\)/sys_ni_syscall\1/g" $scall_file
	done
done
