#!/bin/bash
# unused_func.sh -- print the unused functions
# Usage:
#   $ make 2>typescript or 'script; make; exit'
#   $ unused_func.sh typescript
#
input=$1

[ -z "$input" -a ! -f typescript ] && echo "Usage: $0 input_file" && exit -1

[ -z "$input" -a -f typescript ] && input=typescript

grep "defined but not used" $input | cut -d"'" -f2 | tr -d "'" | sort | uniq -c | sort -g
