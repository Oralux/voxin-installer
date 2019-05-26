#!/bin/bash
# Helper script to export voxin-installer to the x86_64 VM

BASE=$(dirname $(realpath "$0"))
. $BASE/src/conf.inc

upload_to_vm64() {
	SRC="voxin-installer voxin-installer/build/packages/buildroot_2017.02.9.txz voxin-installer/build/.gitignore"
	rsync --exclude build --exclude tmp -aRvz $SRC $VMX86_64:
	rsync  -avR $HOME/VOXIN/voxin-viavoice/build/packages $VMX86_64:	
	ssh $VMX86_64 "mv home/*/VOXIN ."	
}

cd $BASE/..
echo "Build on VM32 ($VMX86) and VM64 ($VMX86_64)? (y|N)"
read a
case "$a" in
	y|Y) a=ok;;
	*) unset a;;
esac
if [ -n "$a" ]; then
	upload_to_vm64
	ssh $VMX86_64 'cd voxin-installer && ./build.sh -u x86'
	ssh $VMX86_64 'cd voxin-installer && ./build.sh -d x86 -t src/list.vv'
else
	echo "Only build on VM64 ($VMX86_64)? (y|N)"
	read a
	case "$a" in
		y|Y) a=ok;;
		*) unset a;;
	esac	
	if [ -n "$a" ]; then
		upload_to_vm64
		ssh $VMX86_64 'cd voxin-installer && ./build.sh -t src/list.vv'
	fi
fi

cd $BASE
./get.sh

