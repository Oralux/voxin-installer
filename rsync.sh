#!/bin/bash
# Helper script to export voxin-installer to the x86_64 VM

BASE=$(dirname $(realpath "$0"))
. $BASE/src/conf.inc

upload_to_vm64() {
#	ssh $VMX86_64 "mv $VMVOXDIR $VMVOXDIR.old.$$; mkdir -p $VMLIBVOXDIR; ln -s $VMLIBVOXDIR/voxind-nve $VMVOXDIR/voxin-nve"	
	SRC="voxin-installer voxin-installer/build/packages/buildroot_2017.02.9.txz voxin-installer/build/.gitignore"
	pushd $HOME/VOXIN
	rsync --exclude build --exclude tmp -aRvz $SRC $VMX86_64:$VMVOXDIR/
	rsync -avR voxin-viavoice/build/packages $VMX86_64:$VMVOXDIR/
	popd
}

echo "Memo: voxind-nve rsynced? (y|N)"
read a
echo "Build on VM32 ($VMX86) and VM64 ($VMX86_64)? (y|N)"
read a
case "$a" in
	y|Y) a=ok;;
	*) unset a;;
esac
if [ -n "$a" ]; then
	upload_to_vm64
	ssh $VMX86_64 "cd $VMVOXDIR/voxin-installer && ./build.sh -u x86"
	ssh $VMX86_64 "cd $VMVOXDIR/voxin-installer && ./build.sh -d x86 -t src/list.vv"
	ssh $VMX86_64 "cd $VMVOXDIR/voxin-installer && ./build.sh -d x86 -t src/list.nve"
else
	echo "Only build on VM64 ($VMX86_64)? (y|N)"
	read a
	case "$a" in
		y|Y) a=ok;;
		*) unset a;;
	esac	
	if [ -n "$a" ]; then
		upload_to_vm64
		ssh $VMX86_64 "cd $VMVOXDIR/voxin-installer && ./build.sh -t src/list.vv"
		ssh $VMX86_64 "cd $VMVOXDIR/voxin-installer && ./build.sh -t src/list.nve"
	fi
fi

./get.sh

