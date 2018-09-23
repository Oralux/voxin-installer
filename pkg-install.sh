#!/bin/bash

. ./conf.inc

if [ "$(id -u)" != "0" ]; then
	echo "install.sh must be run as root"
	exit 1
fi

cd build/packages
dpkg -i libvoxin${MAJ}_$MAJ.$MIN.$REV-${PKG}_amd64.deb
dpkg -i libvoxin${MAJ}-dev_$MAJ.$MIN.$REV-${PKG}_all.deb
dpkg -i voxind_$MAJ.$MIN.$REV-${PKG}_all.deb
