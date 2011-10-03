#!/bin/bash

suffix=$1
[ -z "$suffix" ] && suffix=gc

for arch in x86 powerpc arm mips
do
	echo "Compiling kernel for : "$arch
	CLEAN=$CLEAN V=$V ./build.sh $arch $suffix $2
done
