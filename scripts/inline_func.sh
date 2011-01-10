#!/bin/bash
# inline_func.sh -- print the inlineined functions
# Usage:
#   $ make 2>typescript or 'script; make; exit'
#   $ inline_func.sh typescript
#
input=$1

[ -z "$input" -a ! -f typescript ] && echo "Usage: %0 input_file" && exit -1

[ -z "$input" -a -f typescript ] && input=typescript

grep deprecated $input | egrep "warning|error" | \
	     cut -d"'" -f2 | tr -d "'" | sort | uniq -c | sort -g
