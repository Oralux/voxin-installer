#!/bin/bash -e
# Build voxin-installer by remote host (VM or external board)

BASE=$(dirname $(realpath "$0"))
. $BASE/src/host.inc
. $BASE/src/conf.inc

if [ $# = 0 ] || [ $# -gt 2 ]; then
    echo "usage ./rsync.sh <arch> [<port>]"
    echo "<arch>: x86_64, armv7l or aarch64"
    echo "<port>: ssh port (by default 22)"
    exit 1
fi

ARCH=$1
SSH_PORT=${2:-22}

getBuildServer "$ARCH"
SERVER=$BUILD_USER@$BUILD_HOST
EXTRA_DIR=voxin-viavoice

upload() {
	pushd $HOME/VOXIN
	rsync --delete -aRvz -e "ssh -p $SSH_PORT" voxin-installer $SERVER:$VMVOXDIR/
	rsync --delete -avR voxin-viavoice/build/packages -e "ssh -p $SSH_PORT" $SERVER:$VMVOXDIR/
	popd
}

download() {
	pushd $HOME/VOXIN
	rsync  $1 -av -e "ssh -p $SSH_PORT" $SERVER:$VMVOXDIR/voxin-installer/build/packages/ voxin-installer/sav.voxin_$VOXIN_VERSION
	rsync  $1 -av -e "ssh -p $SSH_PORT" $SERVER:$VMVOXDIR/voxin-installer/check/$VOXIN_VERSION voxin-installer/check/
	popd
}

build() {
    [ "$ARCH" = "x86_64" ] && ssh -p $SSH_PORT $SERVER "set -e; cd $VMVOXDIR/voxin-installer && ./build.sh -d -t src/list.vv"
    ssh -p $SSH_PORT $SERVER "set -e; cd $VMVOXDIR/voxin-installer && ./build.sh -d -t src/list.ve.$ARCH"
    rsync -av -e "ssh -p $SSH_PORT" $SERVER:$VMVOXDIR/voxin-installer/build/tmp/voxin-installer_$VOXIN_VERSION/packages/all/ sd_voxin_tarballs/
}

echo "Memo: voxind-nve rsynced? (y|N)"
read a

upload
build
download
