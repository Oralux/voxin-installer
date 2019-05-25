#!/bin/bash
# Helper script to export voxin-installer to the x86_64 VM

BASE=$(dirname $(realpath "$0"))
. $BASE/src/conf.inc
cd $BASE/..

echo "Rsync to VM?"
read a

SRC="voxin-installer voxin-installer/build/packages/buildroot_2017.02.9.txz voxin-installer/build/.gitignore"
rsync --exclude build --exclude tmp -aRvz $SRC $VMX86_64:
rsync  -avR $HOME/VOXIN/voxin-viavoice/build/packages $VMX86_64:
ssh $VMX86_64 "mv home/*/VOXIN ."

ssh $VMX86_64 'cd voxin-installer && ./build.sh -t src/list.vv'

echo "Rsync from VM?"
read a
rsync  --dry-run -av --exclude .git --exclude build $VMX86_64:voxin-installer/ voxin-installer
rsync  --dry-run -av --exclude .git --exclude build $VMX86_64:voxin-installer/build/packages voxin-installer/build/packages

echo "Really rsync from VM?"
read a
rsync -av --exclude .git --exclude build $VMX86_64:voxin-installer/ voxin-installer
rsync -av --exclude .git --exclude build $VMX86_64:voxin-installer/build/packages/ voxin-installer/build/packages
