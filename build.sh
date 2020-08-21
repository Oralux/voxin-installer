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
-d, --download <arch>  download voxin-installer from a remote machine.
                       <arch> = x86
                       useful to download the 32 bits voxin-installer built on 
                       a remote x86 VM.
                       remote address = variable VMX86 in src/conf.inc
                       (current value = $VMX86)
-u, --upload <arch>    upload voxin-installer to a remote machine and build it.
                       Useful to build the 32 bits voxin-installer on an x86 VM.
                       <arch> = x86
                       remote address = variable VMX86 in src/conf.inc
                       (current value = $VMX86)
--allsd 	       build each supported versions of the speech-dispatcher voxin module.
       		       Without this option, build only the module of the last supported version.
 
Examples:
# build all
 $0

# build all, extract and merge the tarballs in list.txt
# the common tarballs (*all*, *voxind*) are expected to be placed at
# the beginning of the list
 $0 --allsd -t src/list.vv
 $0 --allsd -t src/list.ve

# upload voxin-installer to the X86 VM ($VMX86) and build it
 $0 -u x86

# download the 32 bits voxin-installer from the X86 VM
# and build the resulting installer (x86_64 + x86)
 $0 -d x86 -t src/list.vv

" 

}

unset CLEAN DOWNLOAD HELP BUILDROOT TARBALLS UPLOAD WITH_TTS ALLSD

OPTIONS=`getopt -o cd:hbt:u: --long clean,download:,help,buildroot,allsd,tarballs:,upload: \
             -n "$NAME" -- "$@"`
[ $? != 0 ] && usage && exit 1
eval set -- "$OPTIONS"

while true; do
  case "$1" in
    -b|--buildroot) BUILDROOT=1; shift;;
    -c|--clean) CLEAN=1; shift;;
    -d|--download) DOWNLOAD=$2; shift 2;;
    -h|--help) HELP=1; shift;;
    -t|--tarballs) TARBALLS=$2; shift 2;;
    -u|--upload) UPLOAD=$2; shift 2;;
    --allsd) ALLSD=1; shift;;
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

if [ -n "$TARBALLS" ]; then
	t=$(readlink -e "$TARBALLS") || leave "Error: the tarballs file does not exist (-t $TARBALLS)" 1
	grep voxin-viavoice-all "$t" && WITH_TTS=viavoice
	grep voxin-nve-all "$t" && WITH_TTS=nve
	[ -n "$WITH_TTS" ] || leave "Error: the tarballs file does not include text-to-speech tarballs (-t $TARBALLS)" 1
	TARBALLS=$t
fi

if [ -n "$UPLOAD" ]; then
	unset STATUS
	case $UPLOAD in
		x86) uploadToX86VM && STATUS=0;;
		*) ;;
	esac
	[ -z "$STATUS" ] && leave "Error: can't upload voxin-installer to $VMX86" 1
	exit 0
fi

if [ -n "$DOWNLOAD" ]; then
	unset STATUS
	case $DOWNLOAD in
		x86) downloadVoxinUpdateFromX86VM && STATUS=0;;
		*) ;;
	esac
	[ -z "$STATUS" ] && leave "Error: can't download voxin-installer from $VMX86" 1
fi

checkForeignArch
checkDep
init
[ -n "$BUILDROOT" ] && buildBuildroot

getMinimalRFS32FromBuildroot
getOldLibstdc++
buildInstallerDir

ARCH=$(uname -m)
getLibvoxin "$ARCH" || leave "Error: can't build libvoxin" 1
#getSpeechDispatcherVoxin "$ARCH" "$getLibvoxinRes" master || leave "Error: can't build sd_voxin" 1
version=$(echo $SPEECHD_VOXIN_ALL_VERSIONS | cut -f1 -d" ")
getSpeechDispatcherVoxin "$ARCH" "$getLibvoxinRes" "$version" last || leave "Error: can't build sd_voxin" 1
if [ -n "$ALLSD" ]; then
    version=$(echo $SPEECHD_VOXIN_ALL_VERSIONS| cut -f2- -d" ")
    for i in $version; do
	getSpeechDispatcherVoxin "$ARCH" "$getLibvoxinRes" $i || leave "Error: can't build sd_voxin" 1
    done
fi
buildVoxinModule
getVoxinDoc
buildPackage "$ARCH" || leave "Error: can't build packages" 1

[ "$ARCH" = x86_64 ] && getOtherArch

if [ -n "$TARBALLS" ]; then
	buildReleaseTarball "$TARBALLS" "$ARCH" "$WITH_TTS" || leave "Error: can't build release tarball" 1
fi
