#!/bin/bash -x

BASE=$(dirname $(realpath "$0"))
NAME=$(basename "$0")
. $BASE/src/conf.inc

[ "$UID" = 0 ] && leave "Run this script as non superuser" 1

usage() {
	echo "
Usage: 
 $NAME [options]

This script builds the voxin installer.

Note that a minimal 32 bits root filesystem is needed as input to this
script (see README.org).

Options: 
-c, --clean clean-up:  delete the build directory and object files (buildroot, 
                       tarballs,...)
-h, --help             display this help 
-b, --buildroot        (optionally) build a 32 bits rootfilesystem using 
                       buildroot (https://buildroot.org). 
                       see its configuration in src/buildroot/
-t, --tarballs <file>  extracts the list of supplied tarballs 
                       (voxin-viavoice-all.txz,...) into the root filesystem.
                       <file> contains one tarball per line (full pathname) 

Example:
# build all
 $0

# build all, extract and merge the tarballs in list.txt
 $0 -t src/list.vv

" 

}

unset CLEAN HELP BUILDROOT TARBALLS

OPTIONS=`getopt -o chbt: --long clean,help,buildroot,tarballs: \
             -n "$NAME" -- "$@"`
[ $? != 0 ] && usage && exit 1
eval set -- "$OPTIONS"

while true; do
  case "$1" in
    -b|--buildroot) BUILDROOT=1; shift;;
    -c|--clean) CLEAN=1; shift;;
    -h|--help) HELP=1; shift;;
    -t|--tarballs) TARBALLS=$2; shift 2;;
    --) shift; break;;
    *) break;;
  esac
done

[ -n "$HELP" ] && usage && exit 0

if [ -n "$CLEAN" ]; then
	rm -rf $BASE/build
	find $BASE -name "*~" ! -path "*.git*" -exec rm {} \;
	exit 0
fi

checkDep
init
[ -n "$BUILDROOT" ] && buildBuildroot
getMinimalRFS32FromBuildroot
getOldLibstdc++
buildInstallerDir

unset keepDownloadedSources
ARCH=$(uname -m)
getLibvoxin "$ARCH" "$keepDownloadedSources" || leave "Error: can't build libvoxin" 1
getSpeechDispatcherVoxin "$ARCH" "$getLibvoxinRes" "$keepDownloadedSources" || leave "Error: can't build sd_voxin" 1
buildPackage "$ARCH" || leave "Error: can't build packages" 1
keepDownloadedSources=1

buildReleaseTarball "$TARBALLS" "$ARCH" || leave "Error: can't build release tarball" 1
