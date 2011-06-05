#!/bin/bash
# setup_param.sh -- Insert "#define DO_SETUP_PARAM" to the corresponding file to setup a specific parameter

param=$1

[ -z "$param" ] && echo "Usage: $0 param_name" && exit -1

# Find out which file has defined the parameter

echo "Searching the param: $param ..."

for d in "init kernel mm drivers fs sound arch net block crypto ipc lib security virt"
do
	for f in `find $d -name "*.c"`
	do
		ret=`grep -m 1 -l __setup\(\"$param=\" $f`
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

# Insert "#define DO_SETUP_PARAM" before very the 1st include in the file
line=`grep -m 1 -n "#include" $file | cut -d':' -f1`

ret=`grep DO_SETUP_PARAM $file`
[ -z "$ret" ] && sed -i -e "$line i#define DO_SETUP_PARAM\n" $file

# Prompt
echo "The param $param is setup in $file"
