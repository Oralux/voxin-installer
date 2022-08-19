#!/bin/bash

BASE=$(dirname $(realpath $0))
BASE=${BASE%src/tools}

. $BASE/src/conf.inc

DESTDIR=voxin-$VOXIN_VERSION
rm -rf "$DESTDIR"
for i in nve*z; do
    tar -xf $i;
done

cd "$DESTDIR"
for i in voxin-update-ve*z; do
    tar -xf $i;
done

SOURCE_DATE_EPOCH=$(date +%s)
TARBALL=voxin-update-ve-$VOXIN_VERSION.tgz

# cf https://reproducible-builds.org/docs/archives
tar --sort=name \
      --mtime="@$SOURCE_DATE_EPOCH" \
      --owner=0 --group=0 --numeric-owner \
      --pax-option=exthdr.name=%d/PaxHeaders/%f,delete=atime,delete=ctime \
      -zcf ../"$TARBALL" "$DESTDIR"

cd ..
sha512sum "$TARBALL" > "$BASE/check/$VOXIN_VERSION/voxin-$VOXIN_VERSION.sha512.ve"
