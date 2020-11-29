#!/bin/bash -x
# Helper script to export voxin-installer to the server (VM/board)

BASE=$(dirname $(realpath "$0"))
. $BASE/src/conf.inc

# usage() {
# 	echo "usage ./rsync.sh <arch>"
# 	echo "with <arch>: x86_64, armhf,.."	
# }

# case "$1" in
# 	x86_64)
ARCH=x86_64
SERVER=$VMX86_64
EXTRA_DIR=voxin-viavoice
# 		;;
# 	armhf)
# 		ARCH=armhf
# 		SERVER=$BOARD_ARMHF
# 		unset EXTRA_DIR		   
# 		;;
# 	*) usage; exit 1;;
# esac	

upload_to_server() {
	#	ssh $SERVER "mv $VMVOXDIR $VMVOXDIR.old.$$; mkdir -p $VMLIBVOXDIR; ln -s $VMLIBVOXDIR/voxind-nve $VMVOXDIR/voxin-nve"
	ssh $SERVER "rm -rf $VMVOXDIR/voxin-installer/build"	
	SRC="voxin-installer voxin-installer/build/packages/buildroot_2017.02.9.txz voxin-installer/build/.gitignore"
	pushd $HOME/VOXIN
	rsync --exclude build --exclude tmp -aRvz $SRC $SERVER:$VMVOXDIR/
	rsync -avR voxin-viavoice/build/packages $SERVER:$VMVOXDIR/
	popd
}

echo "Memo: voxind-nve rsynced? (y|N)"
read a
echo "Build on VM32 ($VMX86) and VM64 ($SERVER)? (y|N)"
read a
case "$a" in
	y|Y) a=ok;;
	*) unset a;;
esac
if [ -n "$a" ]; then
	upload_to_server
	ssh $SERVER "cd $VMVOXDIR/voxin-installer && ./build.sh -u x86"
	ssh $SERVER "cd $VMVOXDIR/voxin-installer && ./build.sh -d x86 -t src/list.vv"
	ssh $SERVER "cd $VMVOXDIR/voxin-installer && ./build.sh -d x86 -t src/list.ve"
else
	echo "Only build on VM64 ($SERVER)? (y|N)"
	read a
	case "$a" in
		y|Y) a=ok;;
		*) unset a;;
	esac	
	if [ -n "$a" ]; then
		upload_to_server
		ssh $SERVER "cd $VMVOXDIR/voxin-installer && ./build.sh -t src/list.vv"
		ssh $SERVER "cd $VMVOXDIR/voxin-installer && ./build.sh -t src/list.ve.$ARCH"
	fi
fi

./get.sh

