#!/bin/sh
#
# ld-version [-p|-d|-a] ld-command
#
# Prints the ld version of `ld-command' in a canonical 4-digit form
# such as `0295' for ld-2.95, `0303' for ld-3.3, etc.
#
# With the -p option, prints the patchlevel as well, for example `029503' for
# ld-2.95.3, `030301' for ld-3.3.1, etc.
#
# With the -d option, prints the release date as well, for example `20100303'
# for ld-2.20.1.20100303, etc.
#
# With the -a option, prints all of the above information, etc.
#

if [ "$1" = "-p" ] ; then
	with_patchlevel=1;
	shift;
elif [ "$1" = "-d" ] ; then
	with_date=1;
	shift;
elif [ "$1" = "-a" ] ; then
	with_all=1
	shift;
fi

linker="$*"

if [ ${#linker} -eq 0 ]; then
	echo "Error: No linker specified."
	printf "Usage:\n\t$0 <ld-command>\n"
	exit 1
fi

VERSION=$($linker -v | tr ' ' '\n' | tail -n 1)
MAJOR=$(echo $VERSION | tr '.' '\n' | head -n 1)
MINOR=$(echo $VERSION | tr '.' '\n' | head -n 2 | tail -n 1)
if [ "x$with_patchlevel" != "x" ] ; then
	PATCHLEVEL=$(echo $VERSION | tr '.' '\n' | head -n 3 | tail -n 1)
	printf "%02d%02d%02d\\n" $MAJOR $MINOR $PATCHLEVEL
elif [ "x$with_date" != "x" ] ; then
	RELEASEDATE=$(echo $VERSION | tr '.' '\n' | head -n 4 | tail -n 1)
	printf "%02d%02d%08d\\n" $MAJOR $MINOR $RELEASEDATE
elif [ "x$with_all" != "x" ] ; then
	PATCHLEVEL=$(echo $VERSION | tr '.' '\n' | head -n 3 | tail -n 1)
	RELEASEDATE=$(echo $VERSION | tr '.' '\n' | head -n 4 | tail -n 1)
	printf "%02d%02d%02d%08d\\n" $MAJOR $MINOR $PATCHLEVEL $RELEASEDATE
else
	printf "%02d%02d\\n" $MAJOR $MINOR
fi


