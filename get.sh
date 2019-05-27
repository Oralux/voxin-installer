#!/bin/bash
# Helper script to import voxin-installer from the x86_64 VM

BASE=$(dirname $(realpath "$0"))
. $BASE/src/conf.inc
cd $BASE/..

rsync_from_vm64() {
	rsync  $1 -av --exclude .git --exclude build $VMX86_64:voxin-installer/ voxin-installer
	rsync  $1 -av --exclude .git --exclude build $VMX86_64:voxin-installer/build/packages voxin-installer/build/packages
}


echo "Rsync (dry-run) from VM64 ($VMX86_64)? (y|N)"
read a
case "$a" in
	y|Y) ;;
	*) exit 0;;
esac

rsync_from_vm64 --dry-run

echo "Really rsync from VM64? (y|N)"
read a
case "$a" in
	y|Y) ;;
	*) exit 0;;
esac
rsync_from_vm64

