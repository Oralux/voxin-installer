#!/bin/bash

# this script creates a debug archive in /tmp/voxinDebug.tgz
#

mkdir -p /tmp/voxinDebug/{home,etc,usr}

# copy some speech-dispatcher files (configuration, modules)
cp -a $HOME/.config/speech-dispatcher /tmp/voxinDebug/home
cp -a /etc/speech-dispatcher /tmp/voxinDebug/etc
cp -a /usr/share/speech-dispatcher /tmp/voxinDebug/usr
cp -a /usr/lib64/speech-dispatcher-modules /tmp/voxinDebug/usr

# copy the IBMTTS configuration
cp -a /var/opt/IBM /tmp/voxinDebug

# trace the access to libvoxin
touch /tmp/libvoxin.ok
spd-say -o voxin hello
mv /tmp/libvoxin* /tmp/voxinDebug/

# build the tgz
cd /tmp
tar -zcf voxinDebug.tgz voxinDebug

echo "email please the /tmp/voxinDebug.tgz file to contact@oralux.org"


