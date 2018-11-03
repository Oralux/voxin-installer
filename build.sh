#!/bin/bash -x

BASE=$(dirname $(realpath "$0"))
NAME=$(basename "$0")


usage() {
	echo "
Usage: 
 $NAME [options]

Build the libvoxin packages and their dependencies.

Packages
- libvoxin and voxind,
- (optionally) voxin-rfs32: a minimal 32 bits rootfilesystem supplying standard libraries (built by [[https://buildroot.org][Buildroot]] ) 
- (optionally) voxin-libstdc++: old libstdc++ library needed by ibmtts

Requirement
Firstly, install the voxin-viavoice packages supplied by Oralux.

Voxind will be installed in a 32 bits root filesystem to lower
dependencies issues with the host filesystem.

Note that a minimal 32 bits root filesystem is needed as input to this
script (see README.org).

Options: 
-c	clean-up: delete the build directory and object files
-h  display this help 
-i  install built packages
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

unset CLEAN HELP INSTALL CPP RFS32 

OPTIONS=`getopt -o chilr \
             -n "$NAME" -- "$@"`
[ $? != 0 ] && usage && exit 1
eval set -- "$OPTIONS"

while true; do
  case "$1" in
    -c) CLEAN=1; shift;;
    -h) HELP=1; shift;;
    -i) INSTALL=1; shift;;
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

if [ -n "$INSTALL" ]; then
	$BASE/src/install.sh
	exit 0
fi

[ -n "$RFS32" ] && $BASE/src/rfs32.sh
[ -n "$CPP" ] && $BASE/src/libstdc++.sh
$BASE/src/libvoxin.sh

