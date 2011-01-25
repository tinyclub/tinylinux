#!/bin/bash
# tinysearch.sh -- search the string in the files which have been compiled

[ -z "$1" ] && echo "Usage: $0 <string>" && exit -1
input="$1"

[ -z "$objtree" ] && objtree=./

for f in `find ${objtree} -name "*.o" | sed -e "s/\.o$/\.\[cS\]/"`
do
	if [ -f "$f" ]; then
		grep -nH "$input" "$f"
	fi
done
