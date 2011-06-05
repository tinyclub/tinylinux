#!/bin/bash
# whodef_param.sh -- find out which file has defined a specific parameter

param=$1

[ -z "$param" ] && echo "Usage: $0 param_name" && exit -1

for d in "init kernel mm drivers fs sound arch net block crypto ipc lib security virt"
do
	for f in `find $d -name "*.c"`
	do
		ret=`grep -m 1 -n -H __setup\(\"$param=\" $f`
		if [ -n "$ret" ]; then
			echo $ret
			exit 0
		fi
	done
done
