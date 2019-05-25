#!/bin/bash
# Helper script to import voxin-installer from the x86_64 VM

BASE=$(dirname $(realpath "$0"))
. $BASE/src/conf.inc
cd $BASE/..

echo "Rsync from VM? (dry-run)"
read a
rsync  --dry-run -av --exclude .git --exclude build $VMX86_64:voxin-installer/ voxin-installer
rsync  --dry-run -av --exclude .git --exclude build $VMX86_64:voxin-installer/build/packages voxin-installer/build/packages

echo "Really rsync from VM?"
read a
rsync -av --exclude .git --exclude build $VMX86_64:voxin-installer/ voxin-installer
rsync -av --exclude .git --exclude build $VMX86_64:voxin-installer/build/packages/ voxin-installer/build/packages

