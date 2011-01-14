#!/bin/bash
# setup_param_list.sh -- setup the parameters from users' config

# Debug
Q=

# Get the new param list
new_param_list="$@"
[ -z "$objtree" ] && objtree=./
[ -z "$srctree" ] && srctree=./

# Get the old param list
param_list_bak=$objtree/param_list.bak

# At the first time, save the default param list enabled by kernel as the old
# param list
if [ ! -f $param_list_bak ]; then
	default_param_list=$srctree/usr/default_param_list.txt
	cat $default_param_list | egrep -v "^#|^$" | tr -s ' ' | tr ' ' '\n' > $param_list_bak
fi

# Otherwise, save the old user config in
[ -f $param_list_bak ] && old_param_list=`cat $param_list_bak`

# If no update, exit directly
if [ "$new_param_list" = "$old_param_list" ]; then
	[ -z "$Q" ] && echo "No param list update, exit."
	exit 0
fi

# Save the new param list temply
new_param_list_bak=$objtree/new_param_list.bak
echo $new_param_list | tr ' ' '\n' | sort -u > $new_param_list_bak

# Check which one need to unsetup/setup
unsetup_param_list=`diff $param_list_bak $new_param_list_bak | grep "^<" | tr -d '<\n'`
setup_param_list=`diff $param_list_bak $new_param_list_bak | grep "^>" | tr -d '>\n'`

[ -n "$Q" ] && output="2>&1 > /dev/null"

# Unsetup the deprecated ones
for param in $unsetup_param_list
do
	[ -z "$Q" ] && echo "Unsetup kernel parameter: $param"
	$srctree/scripts/unsetup_param.sh $param $output
done

# Setup the new added ones
for param in $setup_param_list
do
	[ -z "$Q" ] && echo "Setup kernel parameter: $param"
	$srctree/scripts/setup_param.sh $param $output
	[ $? -eq 255 ] && echo "$0: No such param: $param in Documentation/kernel-parameters.txt, please check the spell in CONFIG_PARAM_LIST" \
		&& sed -i -e "/^$param$/d" $new_param_list_bak
done

# Backup the new param list
mv $new_param_list_bak $param_list_bak
