#!/bin/bash -x

# Build a Debian package for each voxin installer in a list
#
# Example of an install tree:
# /opt/oralux/voxin-enu/bin: script used by postinst (preinstall.sh,...)
# /opt/oralux/voxin-enu/lib: content of the installer tarball (voxin-installer.sh,...)
#

usage() {
    echo "usage: $PN <list> <destdir>"
}

BASE="$(dirname $(realpath "$0"))"
PN=$(basename "$0")
LIST=$(realpath "$1")
DESTDIR=$2

[ -f "$LIST" ] || { usage; exit 1; }
[ -d "$DESTDIR" ] || { usage; exit 1; }

[ $UID != 0 ] && { echo "must be run as root"; exit 1; }

cd $BASE
for TARBALL in $(cat "$LIST"); do
    ./buildDebPackage.sh "$TARBALL" "$DESTDIR"
done
