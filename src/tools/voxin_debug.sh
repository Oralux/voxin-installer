#!/bin/bash

# this script creates a debug archive in /tmp/voxinDebug.tgz
#

DIR=/tmp/voxinDebug

mkdir -p $DIR/{home,etc,usr}

# copy some speech-dispatcher files (configuration, modules)
cp -a $HOME/.config/speech-dispatcher $DIR/home
cp -a /etc/speech-dispatcher $DIR/etc
cp -a /usr/share/speech-dispatcher $DIR/usr
cp -a /usr/lib64/speech-dispatcher-modules $DIR/usr

# copy the IBMTTS configuration
cp -a /var/opt/IBM $DIR

# trace the access to libvoxin
touch /tmp/libvoxin.ok
spd-say -o voxin hello
mv /tmp/libvoxin* $DIR/

# orca config
cp $HOME/.local/share/orca/user-settings.conf $DIR/home

# software versions
cp /etc/os-release $DIR/etc
speech-dispatcher -v >> $DIR/report.txt
spd-say -O >> $DIR/report.txt
spd-say -L >> $DIR/report.txt

# build the tgz
cd /tmp
tar -zcf voxinDebug.tgz voxinDebug

echo "email please the $DIR.tgz file to contact@oralux.org"


