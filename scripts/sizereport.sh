#!/bin/bash
# sizereport.sh -- kernel size report
# Author: Wu Zhangjin <wuzhangjin@gmail.com>
# Update: 2011-01-23
# License under GPL 2.0 and later
#

# TODO:
# 1. build a size report system, the size should be considered will the
# details of the function attributes(e.g inline?) and also may be possibly
# reported with the "size --format=SysV"
# 2. This shell script should be able to accept users' input, parse it and take
# corresponding actions:
#    o List built-in.o in a specific directory in reverse order(from big to small)
#      This allows users to see which 'sub-system' should be taken more care.
#      [ built-in.o linked all of the objects under the directory. ]
#    o List *.o(not include built-in.o) in a specific directory(from big to small)
#      This allows users to see which files are should focused on.
#    o List the sections(with "size --format=SysV") of a specific object
#      This allows users to see the details of every sections(function, data,
#      blabla...) in that object
#    o List all of the symbols(function, data, bss) in reverse order
#      This allows users to cut down the size of the files which have biggest
#      influence. This should allows users to select the types of the symbol
#      and allow users to enter into the position of the symbols it defines and
#      even give some suggestion to the users to allow them make some better
#      decision.
#

# References:
# http://http://elinux.org/Kernel_Size_Tuning_Guide

[ -z "$objtree" ] && objtree=./
[ -z "$ARCH" ] && ARCH=mips

# Size of kernel image
echo ""
echo "vmlinux:"
echo "-------------------------------------------------------------------------"
size $objtree/vmlinux | sed -e "s#\.//##"
#size --format=SysV $objtree/vmlinux

# Size of subsystem
echo ""
echo "subsystem:"
echo "-------------------------------------------------------------------------"
size $objtree/*/built-in.o $objtree/arch/$ARCH/built-in.o | grep -v text \
	       | sort -n -r -k 4 | sed -e "s#\.//##"

# Size of objects
echo ""
echo "objects:"
echo "-------------------------------------------------------------------------"
find $objtree/ -name "*.o" | egrep -v "built-in|scripts|vmlinu|piggy|dumy" \
	| xargs -i size {} | grep -v text | sort -n -r -k 4 | sed -e "s#\.//##"

# Size of objects under the specifc directories
for dir in drivers fs kernel mm arch/$ARCH block lib init security net crypto
do
	echo ""
	echo "objects: $dir/: "
	echo "-------------------------------------------------------------------------"
	find $objtree/$dir/ -name "*.o" | egrep -v "built-in|scripts|vmlinu|piggy|dumy" \
		| xargs -i size {} | grep -v text | sort -n -r -k 4 | sed -e "s#\.//##"
done

# Size of data variables: unitilized
echo ""
echo "variables(BSS):"
echo "-------------------------------------------------------------------------"
nm --size -r $objtree/vmlinux | grep " [bB] " | head -200

# Size of data variables: initlialized
echo ""
echo "variables(Data):"
echo "-------------------------------------------------------------------------"
nm --size -r $objtree/vmlinux | grep " [dD] " | head -200

# Size of data variables: Readonly
echo ""
echo "consts(Data):"
echo "-------------------------------------------------------------------------"
nm --size -r $objtree/vmlinux | grep " [rR] " | head -200

# Size of functions
echo ""
echo "functions:"
echo "-------------------------------------------------------------------------"
nm --size -r $objtree/vmlinux | grep " [tT] " | head -200

# Size of symbols(others): Weak functions
echo ""
echo "symbols(weak):"
echo "-------------------------------------------------------------------------"
nm --size -r $objtree/vmlinux | egrep -v " [tTdDbBrR] " | head -200
