#!/bin/bash -x

BASE=$(dirname $(realpath "$0"))
NAME=$(basename "$0")


echo "Work in progress"
exit 1

usage() {
	echo "
Usage: 
 $NAME [options]

This script builds the voxin installer.

Note that a minimal 32 bits root filesystem is needed as input to this
script (see README.org).

Options: 
-c	clean-up: delete the build directory and object files
-h  display this help 
-r  (optionally) build voxin-rfs32
    The standard 32 bits libraries should have been built by Buildroot (https://buildroot.org). 
    see its configuration in src/buildroot/
-l  (optionally) build voxin-libstdc++
    voxin-libstdc++ installs the libstdc++ library in the /opt/voxin/rfs32 directory.

Example:
# build the libvoxin and voxind packages
 $0

# build all packages
 $0 -clr

" 

}

unset CLEAN HELP CPP RFS32 

OPTIONS=`getopt -o chlr \
             -n "$NAME" -- "$@"`
[ $? != 0 ] && usage && exit 1
eval set -- "$OPTIONS"

while true; do
  case "$1" in
    -c) CLEAN=1; shift;;
    -h) HELP=1; shift;;
    -l) CPP=1; shift;;
    -r) RFS32=1; shift;;
    --) shift; break;;
    *) break;;
  esac
done

[ -n "$HELP" ] && usage && exit 0

if [ -n "$CLEAN" ]; then
	rm -rf $BASE/build
	find $BASE -name "*~" ! -path "*.git*" -exec rm {} \;
fi

[ -n "$RFS32" ] && $BASE/src/rfs32.sh
[ -n "$CPP" ] && $BASE/src/libstdc++.sh

# build libvoxin
$BASE/src/libvoxin.sh

