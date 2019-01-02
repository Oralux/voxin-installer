#!/bin/bash

BASE=$(realpath $(dirname "$0")/..)
. $BASE/src/conf.inc


TARBALL="$BR_PKG"

BR_CFG="$BASE/src/buildroot/config.buildroot-${BR_VER}"
BR_DIR="$BASE/build/buildroot"
BR_OUTPUT="$BR_DIR/buildroot-${BR_VER}/output"
# path of the 32 bits rootfilesystem built by buildroot.sh
BR_SRC="$BR_OUTPUT/target"
GLIBC_VER=2.23
BR_GLIBC_LICENSE="$BR_OUTPUT/build/glibc-$GLIBC_VER/LICENSES"


if [ -d "$BR_DIR" ]; then
	echo "$BR_DIR exists!"
	exit 0
fi

free=$(($(stat -f -c %a*%S .)/(1000000)))
if [ $free -lt 3500 ]; then
	echo "Not enough free space: around 3.5 Go of available storage space needed"
	exit 0
fi

mkdir -p "$BR_DIR"
cd "$BR_DIR"
wget "$BR_URL"
tar -xf ./buildroot-${BR_VER}.tar.gz
cd buildroot-${BR_VER}
cp ${BR_CFG} .config

make

if [ -d "$BR_SRC" ]; then
	doc=$BR_SRC/usr/share/doc/glibc-$GLIBC_VER
	mkdir -p "$doc"
	cp $BR_GLIBC_LICENSE $doc/copyright	
	fakeroot bash -c "tar -C \"$BR_SRC\" -Jcf $TARBALL ."	
	echo $TARBALL
fi

