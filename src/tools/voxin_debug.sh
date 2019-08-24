#!/bin/bash

# this script creates a debug archive in /tmp/voxinDebug.tgz
#

DIR=/tmp/voxinDebug

mkdir -p $DIR/{home,etc,usr}

# copy some speech-dispatcher files (configuration, modules)
cp -a $HOME/.config/speech-dispatcher $DIR/home 2>/dev/null
cp -a /etc/speech-dispatcher $DIR/etc 2>/dev/null
cp -a /usr/share/speech-dispatcher $DIR/usr 2>/dev/null
cp -a /usr/lib64/speech-dispatcher-modules $DIR/usr 2>/dev/null

# copy the IBMTTS configuration
cp -a /var/opt/IBM $DIR 2>/dev/null

# trace the access to libvoxin
touch /tmp/libvoxin.ok $HOME/libvoxin.ok
spd-say -o voxin hello
mv /tmp/libvoxin* $DIR/
rm $HOME/libvoxin.ok

# orca config
cp $HOME/.local/share/orca/user-settings.conf $DIR/home 2>/dev/null

# software versions
cp /etc/os-release $DIR/etc
speech-dispatcher -v >> $DIR/report.txt
spd-say -O >> $DIR/report.txt
spd-say -L >> $DIR/report.txt

# build the tgz
cd /tmp
tar -zcf voxinDebug.tgz voxinDebug

echo "email please the $DIR.tgz file to contact@oralux.org"


