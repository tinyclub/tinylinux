#!/bin/bash
# scripts/gcov.sh -- Generate the gcov profiling result automatically
# Author: Wu Zhangjin <wuzhangjin@gmail.com>
# Update: 2011-01-16

# Usage:
#
#      1. Download linux source code(>31?) to the target machine
#      2. Compile it with the following two options
#           CONFIG_DEBUG_FS=y
#           CONFIG_GCOV_KERNEL=y
#           CONFIG_GCOV_PROFILE_ALL=y
#
#          Note:
#		+ If your board doesn't support CONFIG_GCOV_PROFILE_ALL
#		  Please add it in kernel/gcov/Kconfig and validate it
#		  If it doesn't boot, you may need to comment out some
#		  files, please refer to the following commit and add
#		  mark out your own files/dirs for Gcov:
#			"gcov: enable GCOV_PROFILE_ALL for x86_64"
#		+ Please build it under the source code directory,
#                which will be easier for later usage.
#      3. Mount the debugfs file system
#		$ mount -t debugfs none /sys/kernel/debug
#      4. Get the profile result with this shell script
#		Run this shell script under the build/source code dir
#      5. Recompile the kernel with GCOV_KERNEL_OPT option
#         With this profiling result
#
# Reference: Documentation/gcov.txt

# Init
debugfs=/sys/kernel/debug/
srctree=$PWD/
debugfs_gcov=$debugfs/gcov/$srctree

# Mount the debugfs
mount -t debugfs none $debugfs

# Gcov on all compiled files

gcda_list=$(mktemp)

find $debugfs_gcov -name "*.gcda" > $gcda_list

gcov_result=$(mktemp)
echo > $gcov_result

while read i
do
	# Copy .gcda from debugfs
	src_gcda=$i
	src_c=$(echo $i | sed -e "s/gcda/c/" | sed -e "s#$debugfs_gcov##g")
	dst_gcda_dir=`dirname $(echo $src_gcda | sed -e "s#$debugfs_gcov##g")`

	echo cp $src_gcda $dst_gcda_dir/
	cp $src_gcda $dst_gcda_dir/

	# Gcov on them
	echo gcov -o $dst_gcda_dir $src_c
	gcov -o $dst_gcda_dir $src_c >> $gcov_result
done < $gcda_list

echo "========================Gcov Report================================="
echo "Total Files : `wc -l $gcda_list`"
echo "Details in  : $gcov_result"
echo ""
echo "Now, you can enable GCOV_KERNEL_OPT and recompile your kernel"
echo "===================================================================="
