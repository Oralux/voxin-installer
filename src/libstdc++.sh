#!/bin/bash -xe

BASE=$(realpath $(dirname "$0")/..)
. $BASE/src/conf.inc

workdir="$TMPDIR/libc++"
rm -rf "$workdir"
mkdir -p "$workdir/rfs"
cd "$workdir"
wget $LIBSTDCPP_URL
cd rfs
dpkg-deb -e ../$(basename $LIBSTDCPP_URL)
dpkg-deb -x ../$(basename $LIBSTDCPP_URL) .
rm -rf DEBIAN

cp -a * $RFS_DIR/$RFS32/

# releasing the tarball
TARBALL=$PKGDIR/voxin-rfs32.txz
fakeroot bash -c "tar -C \"$RFS_DIR\" -Jcf $TARBALL ."	
echo $TARBALL
