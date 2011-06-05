#!/bin/bash
# unsetup_param.sh -- unsetup a specific parameter with the new empty __setup()

param=$1

[ -z "$param" ] && echo "Usage: $0 param_name" && exit -1

# Find out which file has defined the parameter

echo "Searching the param: $param ..."

for d in "init kernel mm drivers fs sound arch net block crypto ipc lib security virt"
do
	for f in `find $d -name "*.c"`
	do
		ret=`grep -m 1 -l __raw_setup\(\"$param=\" $f`
		if [ -n "$ret" ]; then
			file=$ret
			break;
		fi
	done
	if [ -n "$ret" ]; then
		break;
	fi
done

echo "The param is found in $file."

# Use __setup() to unsetup a specific parameter
line=`grep -m 1 -n __raw_setup\(\"$param=\" $file | cut -d':' -f1`
sed -i -e "$line s/__raw_setup/__setup/g" $file

# Prompt
echo "The param $param is unsetup with the empty __setup() in $file"
