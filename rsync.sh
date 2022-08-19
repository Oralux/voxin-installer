#!/bin/bash -e
# Build voxin-installer by remote host (VM or external board)

BASE=$(dirname $(realpath "$0"))
. $BASE/src/host.inc
. $BASE/src/conf.inc

if [ $# != 1 ]; then
    echo "usage ./rsync.sh <arch>"
    echo "<arch>: x86_64, armv7l or aarch64"
    exit 1
fi

ARCH=$1
getBuildServer "$ARCH"
SERVER=$BUILD_USER@$BUILD_HOST
EXTRA_DIR=voxin-viavoice

upload() {
	pushd $HOME/VOXIN
	rsync --delete -aRvz voxin-installer $SERVER:$VMVOXDIR/
	rsync --delete -avR voxin-viavoice/build/packages $SERVER:$VMVOXDIR/
	popd
}

download() {
	pushd $HOME/VOXIN
	rsync  $1 -av $SERVER:$VMVOXDIR/voxin-installer/build/packages/ voxin-installer/sav.voxin_$VOXIN_VERSION
	rsync  $1 -av $SERVER:$VMVOXDIR/voxin-installer/check/$VOXIN_VERSION voxin-installer/check/
	popd
}

build() {
    [ "$ARCH" = "x86_64" ] && ssh $SERVER "set -e; cd $VMVOXDIR/voxin-installer && ./build.sh -t src/list.vv"
    ssh $SERVER "set -e; cd $VMVOXDIR/voxin-installer && ./build.sh -t src/list.ve.$ARCH"
}

echo "Memo: voxind-nve rsynced? (y|N)"
read a

upload
build
download
