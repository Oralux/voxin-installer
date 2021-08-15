# 2021, Gilles Casse <gcasse@oralux.org>
#

# This script downloads and installs the last version of Voxin.
# The previous version will be kept so that a rollback is possible if needed.
#

URL=https://oralux.org/download.php
VERSION=1.0

TOPDIR=${1:-"/"}
RFSDIR=opt/oralux/voxin

BASE=$(dirname $(readlink -f $0))
cd $BASE

[ "$UID" != "0" ] && "Please run this script as superuser" && exit 0

# determine the current version installed
OLD_VER=$(awk -F= '/tag/{print $2}' "$TOPDIR"/"$RFSDIR"/share/doc/voxin-installer/sources.ini)
[ -z "$OLD_VER" ] && echo "incompatible version of voxin installed" && exit 1

md5=$(ls /opt/oralux | while read line; do a=$(echo "$line" | md5sum); echo ${a:0:4}; done)
md5=$(echo $md5 | tr ' ' '_')
URL="$URL?u=${OLD_VER}&m=$md5"

# create a local download directory
WORKDIR="$BASE"/voxin-updater/$(date +%F_%H:%M)
mkdir -p "$WORKDIR" || { echo "Can't create $WORKDIR"; exit 1; }
cd "$WORKDIR"
wget -N --content-disposition "$URL"
[ $? != 0 ] && echo "Download error, url=$URL" && exit 1

tar -xf voxin*z
cd voxin*/voxin-update*
./voxin-installer.sh --update
