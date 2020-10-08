#!/bin/bash -x

BASE=$(dirname $(realpath $0))
BASE=${BASE%src/tools}

. $BASE/src/conf.inc

for i in nve*z; do
    tar -xf $i;
done

cd voxin-$VOXIN_VERSION

for i in voxin-update-ve*z; do
    tar -xf $i;
done

SOURCE_DATE_EPOCH=$(date +%s)

# cf https://reproducible-builds.org/docs/archives
tar --sort=name \
      --mtime="@$SOURCE_DATE_EPOCH" \
      --owner=0 --group=0 --numeric-owner \
      --pax-option=exthdr.name=%d/PaxHeaders/%f,delete=atime,delete=ctime \
      -zcf ../voxin-update-ve-$VOXIN_VERSION.tgz voxin-$VOXIN_VERSION

