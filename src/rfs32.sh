#!/bin/bash -xe

BASE=$(realpath $(dirname "$0")/..)
. $BASE/src/conf.inc

#NAME=voxin-rfs32
#TARBALL=$PKGDIR/${NAME}.txz

if [ ! -f "$BR_PKG" ]; then
	echo "
Buildroot tarball not found: $BR_PKG
You may want to run buildroot.sh
"
	exit 1
fi

rm -rf "$RFS_DIR"
mkdir -p "$RFS_DIR"

LIST="./etc/issue ./etc/os-release ./lib/ld-2.23.so ./lib/ld-linux.so.2 ./lib/libc-2.23.so ./lib/libc.so.6 ./lib/libdl-2.23.so ./lib/libdl.so.2 ./lib/libm-2.23.so ./lib/libm.so.6 ./lib/libpthread-2.23.so ./lib/libpthread.so.0 ./usr/share/doc/glibc*/copyright"
rfs32=$RFS_DIR/$RFS32
mkdir -p $rfs32
tar -C "$rfs32" -xf "$BR_PKG" --wildcards $LIST
find $rfs32 -type f -executable ! -name "ld-*" -exec chmod a-x {} \;

#fakeroot bash -c "tar -C $dst -Jcf $TARBALL ."
#echo $TARBALL


