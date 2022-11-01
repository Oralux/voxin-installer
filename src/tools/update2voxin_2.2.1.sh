#!/bin/bash -e

PV=2.2.1
ARCH=i386
#[ "$(uname -m)" = "x86_64" ] && ARCH=amd64

# Install the old voxin 2.2 (IBM TTS) on Debian 11 (x86/32 bits).
#
# This script updates voxin 2.2 to $PV, version which is compatible with the 32 bits Debian speech-dispatcher-ibmtts package.
#
# INSTALL:
# 1. Copy this script in the voxin 2.2 uncompressed archive (in the directory containing voxin-installer.sh)
# 2. Launch it as superuser
#    ./update2voxin_2.2.1.sh
# 3. Once completed, install the speech-dispatcher-ibmtts package from Debian (contrib)
#    apt update
#    apt install speech-dispatcher-ibmtts
# 4. check
#    spd-say -o ibmtts hello
#
if [ "$UID" != 0 ]; then
    echo "This script must be launched as superuser"
    exit 0
fi

dpkg -s voxin 1>/dev/null || { echo "install voxin 2.2 before launching this script"; exit 0; }
x=$(dpkg -l voxin | grep voxin| awk '{print $3}')
if [ "$x" != "2.2" ]; then
    echo "Install voxin 2.2 before launching this script"; exit 0;
fi

if [ ! -e packages/deb/voxin_2.2_$ARCH.deb ]; then
    echo "This script must be copied in the voxin2.2 uncompressed archive (in the directory containing voxin-installer.sh )";  exit 0;
fi    

TEMP=tmp_voxin
rm -rf $TEMP
mkdir -p $TEMP/deb/{DEBIAN,lib/i386-linux-gnu}
cp packages/deb/voxin_2.2_$ARCH.deb $TEMP
cd $TEMP/deb

dpkg-deb -x ../voxin_2.2_$ARCH.deb .
dpkg-deb -e ../voxin_2.2_$ARCH.deb .

rm -rf etc
rm -rf usr/lib/speech-dispatcher-modules
rm -rf usr/share/speech-dispatcher
rm control md5sums

ln -s /opt/oralux/voxin/rfs32/usr/lib/libstdc++-libc6.2-2.so.3 lib/i386-linux-gnu/
ln -s /opt/IBM/ibmtts/lib/libetidev.so lib/i386-linux-gnu/



find . -type f | xargs md5sum >> DEBIAN/md5sums
SIZE=$(du -scb . | head -n1 | cut -f1)

cat <<EOF>DEBIAN/control
Package: voxin
Version: $PV
Architecture: $ARCH
Maintainer: voxin-installer <contact@oralux.org>
Installed-Size: $SIZE
Depends: libc6
Section: sound
Conflicts: libvoxin1, libvoxin1-dev, voxind
Replaces: libvoxin1, libvoxin1-dev, voxind
Description: Voxin pseudo package
 This package is compatible with the Debian speech-dispatcher-ibmtts package.
 Voxin is an easily installable add-on which provides yet another text-to-speech 
 to blind users of GNU/Linux. This package provides symbolic links to voxin.

EOF

dpkg-deb -b . ../voxin_${PV}_${ARCH}.deb
sha256sum ../voxin_${PV}_${ARCH}.deb > ../voxin_${PV}_${ARCH}.deb.sha256

dpkg -i ../voxin_${PV}_${ARCH}.deb

printf "OK: voxin $PV installed.\n\
Now you may want to install the speech-dispatcher-ibmtts package\n\
 - add contrib to /etc/apt/sources.list\n\
 - apt update\n\
 - apt install speech-dispatcher-ibmtts\n\
"
