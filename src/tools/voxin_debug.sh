#!/bin/bash

# This script collects info in the compressed file /tmp/voxinDebug.tgz
# about the installation of voxin, the configuration of
# speech-dispatcher and orca
#

DIR=/tmp/voxinDebug
REPORT=$DIR/report.txt

mkdir -p $DIR/{home,etc,usr}

voxin-say -L >> "$REPORT"

echo "# check the installation of voxin" >> "$REPORT"
find /opt/oralux -type f | while read i; do md5sum $i >> "$REPORT"; done
cp -a /var/opt/IBM $DIR 2>/dev/null

echo "# copy some speech-dispatcher files (configuration, modules)" >> "$REPORT"
cp -a $HOME/.config/speech-dispatcher $DIR/home 2>/dev/null
cp -a /etc/speech-dispatcher $DIR/etc 2>/dev/null
cp -a /usr/share/speech-dispatcher $DIR/usr 2>/dev/null
cp -a /usr/lib*/speech-dispatcher-modules/sd_voxin $DIR/usr 2>/dev/null

echo "# check if speech-dispatcher lists voxin" >> "$REPORT"
spd-say -O >> "$REPORT"
spd-say -o voxin -L >> "$REPORT"

echo "# software versions" >> "$REPORT"
speech-dispatcher -v >> "$REPORT"

cp /etc/os-release $DIR/etc

# build the tgz
cd /tmp
tar -zcf voxinDebug.tgz voxinDebug

echo "email please the $DIR.tgz file to contact@oralux.org"


