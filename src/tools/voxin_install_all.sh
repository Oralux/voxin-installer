#!/bin/bash

VERSION=3.4

BUILD_DIR="$HOME"/.local/share/voxin.build

usage() {
    echo "
Usage: 
voxin_install_all.sh [OPTION] [SRC_DIR]

Install all the Voxin tarballs included in the SRC_DIR directory.
Note that these tarballs will first be unpacked in the $BUILD_DIR directory.

If the -u option is supplied, all the Voxin voices are uninstalled.
Then optionally, the tarballs included in SRC_DIR are installed.

If this script is launched as super user, the voices will be installed system-wide.
If launched as regular user, the voices will be installed locally.

"
}

uncompress() {
    local list=$1
    local dest=$2
    local i
    [ ! -d "$dest" ] && return 1
    for i in $list; do
	echo "Uncompressing $i"
	tar -xf "$i" -C "$dest"
    done
}

install() {
    local dir=$1
    local i
    [ ! -d "$dir" ] && return 1
    for i in $(find "$dir" -name voxin-installer.sh); do
	j="${i%/*}"
	echo "Installing ${j##*/}"
	"$i" -l
    done
}

uninstall() {
    local dir=$(realpath $1)
    local i
    [ ! -d "$dir" ] && return 1
    for i in $(find "$dir" -name voxin-installer.sh); do
	j="${i%/*}"
	echo "Uninstalling ${j##*/}"
	"$i" -lu
	rm -rf "$BUILD_DIR"/../voxin.build
	break
    done
}


[ $# = 0 ] || [ $# -gt 2 ] && { usage; exit 1; }

if [ "$1" = "-u" ]; then
    [ ! -d "$BUILD_DIR" ] && { echo "Error: build dir not found ($BUILD_DIR)" && exit 1; };
    uninstall "$BUILD_DIR"
    shift
    [ -z "$1" ] && exit 0
fi

[ ! -d "$BUILD_DIR" ] && mkdir -p "$BUILD_DIR"

SRC_DIR=$(realpath "$1")
[ ! -d "$SRC_DIR" ] && { usage; exit 1; }

cd "$SRC_DIR"
LIST=$(ls voxin*-"$VERSION".*tgz) 
[ -z "$LIST" ] && { usage; exit 1; }

uncompress "$LIST" "$BUILD_DIR"
install "$BUILD_DIR"
