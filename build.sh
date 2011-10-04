#!/bin/bash
# Usage: ./build.sh <ARCH> <orig|gc> <arg for -j>
#  e.g.: ./build.sh arch gc 3
#  e.g.: CLEAN=1 ./build.sh arch gc 3

# output: $ARCH.$SUFFIX/
#

ARCH=$1
SUFFIX=$2
[ -z "$SUFFIX" ] && SUFFIX=gc
TARGET=$ARCH.$SUFFIX

echo "** target: $TARGET"

case $ARCH in
	arm)
		CROSS_COMPILE=arm-linux-gnueabi-
		;;
	powerpc)
		CROSS_COMPILE=powerpc-linux-gnu-
		;;
	mips)
		CROSS_COMPILE=mipsel-unknown-linux-uclibc-
		vmlinuz=vmlinuz
		;;
	*)
		ARCH=x86
		CROSS_COMPILE=
		;;
esac

echo "** cross compiler: `${CORSS_COMPILE}gcc --version | head -n 1` "
echo "** cross linker  : `${CORSS_COMPILE}ld -v` "

# Clean the old results
if [ -n "$CLEAN" ]; then
echo "** make clean ..."
make clean ARCH=$ARCH
fi

# Configure with the defaut gc_sections_defconfig
defcfg=gc_sections_defconfig
defcfg_bak=$ARCH.defconfig

echo "** make config with $defcfg ..."

mkdir -p $TARGET
if [[ "$SUFFIX" == "gc" ]]; then
	make ARCH=$ARCH $defcfg
	cp .config $TARGET/$defcfg
	cp .config $defcfg_bak
else
	cp $defcfg_bak arch/$ARCH/configs/$defcfg
	make ARCH=$ARCH $defcfg
fi

# Compile
echo "** compiling with ${CROSS_COMPILE}gcc for $ARCH ..."
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE -j$3 V=$V $vmlinuz

# Save the kernel image and dump size information
echo "** saving kernel image and dump size info ..."
vmlinux=vmlinux.$SUFFIX
vmlinux_strip_s=$vmlinux.strip.s
vmlinux_strip_x=$vmlinux.strip.x

cp vmlinux $TARGET/$vmlinux
cp vmlinux $TARGET/$vmlinux_strip_s
cp vmlinux $TARGET/$vmlinux_strip_x
${CROSS_COMPILE}strip -s $TARGET/$vmlinux_strip_s
${CROSS_COMPILE}strip -x $TARGET/$vmlinux_strip_x

size_info_file=$TARGET/size_info_file.$TARGET
echo "** size info saved in: "$TARGET
echo "============================================"
echo "" > $size_info_file
for i in $vmlinux $vmlinux_strip_s $vmlinux_strip_x
do
ls -l $TARGET/$i >> $size_info_file
size $TARGET/$i >> $size_info_file
done
cat $size_info_file
echo "============================================"

# Backup the compressed kernel image
echo "saving the compressed kernel image"

case $ARCH in
	arm)
		cp -v arch/arm/boot/zImage zImage.$ARCH
		;;
	x86)
		cp -v arch/x86/boot/bzImage bzImage.$ARCH
		;;
	mips)
		cp -v vmlinuz vmlinuz.$ARCH
		;;
	*)
		cp -v vmlinux vmlinux.$ARCH
		;;
esac
