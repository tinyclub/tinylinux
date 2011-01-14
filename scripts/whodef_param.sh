#!/bin/bash
# whodef_param.sh -- find out which file has defined a specific parameter

param=$1

[ -z "$param" ] && echo "Usage: $0 param_name" && exit -1

# before real search, check it in Documentation/kernel-parameters.txt
# We ignore the ones which has not been listed in Documentation/kernel-parameters.txt
ret=`egrep -m 1 "^[[:space:]]$param[[:space:]]|^[[:space:]]$param=" Documentation/kernel-parameters.txt`
if [ -z "$ret" ]; then
	echo "No such param: $param"
	exit -1
else
	# Parse the target directory from Documentation/kernel-parameters.txt
	# Only consider the popular ones at first
	where=`echo $ret | sed -e "s/.*\[\(.*\)\].*/\1/g" | tr ',' ' ' | tr '=' ' '`
	for w in $where
	do
		case $w in
			X86*|IA*|AMD|Intel) W_ARCH=x86
				;;
			MIPS) W_ARCH=mips
				;;
			ARM) W_ARCH=arm
				;;
			PPC) W_ARCH=powerpc
				;;
			SH) W_ARCH=sh
				;;
			S390) W_ARCH=s390
				;;
			LIBATA) W_DRV=ata
				;;
			MTD) W_DRV=mtd
				;;
			NET*)
				W_NET=net
				;;
			MM|NUMA|SLAB)
				W_MM=mm
				;;
			SCSI)
				W_DRV=scsi
				;;
			ALSA|OSS)
				W_SND=sound
				;;
			PCI*)
				W_DRV=pci
				;;
			SELINUX|SECURITY)
				W_SEC=security
				;;
		esac
	done
fi

dirs="init kernel arch/$ARCH mm block ipc crypto net lib security virt sound drivers fs"

if [ -n "$W_ARCH" -o -n "$W_MM" -o -n "$W_SND" -o -n "$W_DRV" -o -n "$W_NET" -o -n "$W_SEC"  ]; then
	dirs=
	[ -n "$W_ARCH" -a "$W_ARCH" = "$ARCH"  ] && dirs="$dirs arch/$ARCH"
	[ -n "$W_MM" ] && dirs="$dirs mm"
	[ -n "$W_SND" ] && dirs="$dirs sound"
	[ -n "$W_NET" ] && dirs="$dirs net"
	[ -n "$W_SEC" ] && dirs="$dirs security"
	[ -n "$W_DRV" ] && dirs="$dirs drivers/$W_DRV"
fi

echo "Searching the param: $param ... (May need several minutes, please wait...)"

for d in $dirs
do
	for f in `find $d -name "*.c"`
	do
		# Is there a _setup("param?
		param_type="_setup"
		ret=`grep -m 1 -nH $param_type\(\"$param $f`
		if [ -n "$ret" ]; then
			file=$ret
			echo $file
			exit 0;
		else
			# Is there a early_param("param
			param_type="early_param"
			ret=`grep -m 1 -nH $param_type\(\"$param $f`
			if [ -n "$ret" ]; then
				file=$ret
				echo $file
				exit 0;
			fi
		fi
	done
done
