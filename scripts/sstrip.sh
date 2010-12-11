#!/bin/bash
# sstrip.sh -- strip the section table of an elf file
#
# Copyright (C) 2010 Wu Zhangjin, wuzhangjin@gmail.com
# Licensed under the GPLv2
#
# Since the section table is useless for the embedded device, it can be
# stripped out.
#
# Note: Some bootloader may check the section table but most of the time, it
# may be not really used, If it really need the section table, it may need the
# decompressed kernel image.

# Usage

function usage
{
cat <<EOF

	# sstrip.sh -- strip the section table of an elf file

	# Input: elf file
	# Output: truncated elf file without the section table
	# Usage: sstrip.sh /path/to/image

EOF
}

# Do some necessary check
IMAGE=$1

[ -z "${IMAGE}" ] && echo "$0 : No indicated file to be stripped" && usage && exit -1
[ ! -f "${IMAGE}" ] && echo "$0 : ${IMAGE} : No such file" && exit -1
FILE_TYPE=`dd if=${IMAGE} bs=1 skip=1 count=3 2>/dev/null`
[ "xELF" != "x${FILE_TYPE}" ] && echo "$0: ${IMAGE} is not an ELF file" && exit -1

[ "x${V}" == "x1" ] && orig_filesz=`wc -c ${IMAGE} | cut -d' ' -f1`

# Get the offset of the section table, here get the end of the program section
filesz=$((`${OBJDUMP} -p ${IMAGE} | grep -m1 filesz | tr -s ' ' | cut -d' ' -f3`))

# Truncate it via the dd tool
dd if=/dev/null bs=1 of=${IMAGE} seek=${filesz} 2>/dev/null

# Debug
if [ "x${V}" == "x1" ]; then
	echo "----------------------------------------------------------------"
	echo "Strip the section table at ${filesz} of ${IMAGE}"
	echo "----------------------------------------------------------------"
	echo "       sstrip: $0"
	echo "      objdump: ${OBJDUMP}"
	echo "original size: ${orig_filesz}"
	echo "current  size: ${filesz}"
	echo "reduced  size: $((${orig_filesz} - ${filesz}))"
fi
