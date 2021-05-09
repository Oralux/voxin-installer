#!/bin/bash -x

TARBALL=$(realpath "$1")
echo TODO UID
#[ $UID != 0 ] && { echo "must be run as root (or update this script to use fakeroot)"; exit 1; }

# Install tree
# /opt/oralux/voxin-enu/bin/preinstall.sh
# /opt/oralux/voxin-enu/lib/voxin-installer.sh

# findVoiceTarball() {
#     local basedir="$1"
#     VOICE_TARBALL=$(find "$basedir" -path "*/all/voxin-ve-*txz" -o -path "*/all/voxin-viavoice*txz" ! -path "*/all/voxin-viavoice-all_*txz")
#     [ -f "$VOICE_TARBALL" ]
# }

# NAME: package name, also target install dir (/opt/oralux/$NAME)
getNameVersion() {
    local tarball="$1"
    [ -n "$tarball" ] || return 1
    local name1=$(echo $tarball | sed -e 's+.*/++' -e 's/.x86_64//' -e 's/\.tgz//')
    NAME=$(echo $name1 | sed -e 's/-[[:digit:]].*//')
    VERSION=$(echo $name1 | sed -e "s/$NAME-//")
    [ -n "$NAME" ] && [ -n "$VERSION" ]
}

getNameVersion "$TARBALL" || { echo "erroneous arg (tarball expected)"; exit 1; }

BASE="$(dirname $(realpath "$0"))"
#PN=$(basename "$0")

RFS=opt/oralux/$NAME

BUILD=$(mktemp -d --suffix=gui_inst)
[ $? != 0 ] && { echo "error: mktemp"; exit 1; }
[ -z "$BUILD" ] && { echo "error: mktemp"; exit 1; }

mkdir -p "$BUILD"/data/"$RFS"/{bin,lib}
mkdir    "$BUILD"/DEBIAN

install -m755 preinstall*.sh "$BUILD/data/$RFS/bin"
install -m755 "$BASE"/postinst "$BUILD"/DEBIAN

# data.tar.xz
#touch data/$RFS/{LICENSE,README}
cd "$BUILD"/data/"$RFS"/lib
tar -xf "$TARBALL"
# remove the 2 first level (voxin-3.3rc4/voxin-enu-3.3rc4 or voxin-3.3rc4/voxin-american-english-samantha-compact-3.3rc4)
dir="voxin-${VERSION}/${NAME}-${VERSION}"
[ ! -d "$dir" ] && { echo "can't find directory $dir"; exit 1; }
[ ! -f "$dir"/voxin-installer.sh ] && { echo "can't find voxin-installer.sh in $dir"; exit 1; }
mv "$dir"/* .
rm -rf "$dir"

#change the pathname of the logfile
sed -i 's@LOG="$BASE/log/voxin.log"@LOG="/tmp/voxin-installer.log.$$"@' common/init.inc

cd "$BUILD"/data

## --------
## not implemented because conflict if voice files already installed by
## manual method/withoutdebian package)
## --------
#findVoiceTarball .
#[ $? != 0 ] && { echo "error: voice tarball not found in $TARBALL"; exit 1; }
#tar -C "$BUILD"/data -xf "$VOICE_TARBALL"
#rm "$VOICE_TARBALL"

chown -R root.root "$BUILD"/data

#fakeroot bash -c "tar -Jcf ../data.tar.xz ."
tar -Jcf ../data.tar.xz .

# control.tar.xz
find . -type f | xargs md5sum >> ../DEBIAN/md5sums
size=$(du -sck . | head -n1 | cut -f1)
cat<<EOF>../DEBIAN/control
Package: $NAME
Version: $VERSION
Architecture: all
Maintainer: voxin-installer <contact@oralux.org>
Installed-Size: $size
Section: sound
Description: this package helps to install voxin
EOF
cd ..
#fakeroot bash -c "tar -C DEBIAN -Jcf control.tar.xz control md5sums postinst"
tar -C DEBIAN -Jcf control.tar.xz control md5sums postinst

echo 2.0 > debian-binary
ar -r "${BUILD}"/${NAME}_${VERSION}_all.deb debian-binary control.tar.xz data.tar.xz 


#rm -rf $BUILD

