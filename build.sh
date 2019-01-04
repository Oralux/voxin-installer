#!/bin/bash -xv

BASE=$(dirname $(realpath "$0"))
NAME=$(basename "$0")
. $BASE/src/conf.inc

usage() {
	echo "
Usage: 
 $NAME [options]

This script builds the voxin installer.

Note that a minimal 32 bits root filesystem is needed as input to this
script (see README.org).

Options: 
-c	clean-up: delete the build directory and object files (buildroot, tarballs,...)
-h  display this help 
-b  (optionally) build a 32 bits rootfilesystem using buildroot (https://buildroot.org). 
    see its configuration in src/buildroot/


Example:
# build all
 $0

" 

}

unset CLEAN HELP BUILDROOT 

OPTIONS=`getopt -o chb \
             -n "$NAME" -- "$@"`
[ $? != 0 ] && usage && exit 1
eval set -- "$OPTIONS"

while true; do
  case "$1" in
    -c) CLEAN=1; shift;;
    -h) HELP=1; shift;;
    -b) BUILDROOT=1; shift;;
    --) shift; break;;
    *) break;;
  esac
done

[ -n "$HELP" ] && usage && exit 0

## TODO
# if [ -n "$CLEAN" ]; then
# 	rm -rf $BASE/build
# 	find $BASE -name "*~" ! -path "*.git*" -exec rm {} \;
# fi


mkdir -p "PKGDIR" "DWLDIR" "RFS32"
[ -n "$BUILDROOT" ] && buildBuildroot
getMinimalRFS32FromBuildroot
getOldLibstdc++
getLibvoxin

echo "rfs: $RFS_DIR"
