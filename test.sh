#!/bin/bash -e
# Test install by regular user in remote target (VM or external board)

BASE=$(dirname $(realpath "$0"))
. $BASE/src/host.inc
. $BASE/src/conf.inc

if [ "$#" != 1 ]; then
    echo "usage: ./test.sh SERVER (e.g. SERVER=user1@debian8-x86-64.local)"
    exit 1
fi
SERVER=$1

cd $HOME/VOXIN/tmp/$VOXIN_VERSION
LIST=$(ls *z | tr '\n' ' ')
[ -n "$LIST" ] || exit 1

scp $LIST $SERVER:
ssh $SERVER << EOF 
set -ex
DESTDIR="voxin-$VOXIN_VERSION"
rm -rf "\$DESTDIR"

for tarball in $LIST; do tar -xf \$tarball; done

cd "\$DESTDIR"
for dir in voxin*; do
  pushd "\$dir"
  ./voxin-installer.sh -d INSTALL
  popd
done

EOF
