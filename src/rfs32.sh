#!/bin/bash -xe

BASE=$(realpath $(dirname "$0")/..)
. $BASE/src/conf.inc

NAME=voxin-rfs32
VERSION=1.0-1

FILES=$TMPDIR/$NAME.files
#RFS32=/opt/voxin/rfs32

getChangelog() {
	local name=$1
	local version=$2
	local file=$3
	local date=$(LANG=en_US.UTF-8 date -R)
	cat <<EOF>$file
$name ($version) UNRELEASED; urgency=medium

  * Initial archive from buildroot (2017.02.9)

 -- Gilles Casse <gcasse@oralux.org>  $date

EOF
	
}


#rm -rf "$TMPDIR" "$PKGDIR"
rm -rf "$TMPDIR" "$PKGDIR/${NAME}_${VERSION}_all.deb"
mkdir -p "$PKGDIR" "$TMPDIR"


cat<<EOF>"$FILES"
lib/libdl-2.23.so
lib/libdl.so.2
lib/libpthread-2.23.so
lib/ld-2.23.so
lib/libc-2.23.so
lib/libpthread.so.0
lib/ld-linux.so.2
lib/libm.so.6
lib/libc.so.6
lib/libm-2.23.so
etc/issue
etc/os-release
EOF

dst=$TMPDIR/$NAME	
rfs32=$dst/$RFS32

mkdir -p "$rfs32"
rsync -av --files-from="$FILES" "$BR_SRC/" $rfs32/
find $rfs32 -type f -executable ! -name "ld-*" -exec chmod a-x {} \;

doc=$(dirname $rfs32)/share/doc/$NAME
mkdir -p "$doc"
cp $BR_GLIBC_LICENSE $doc/copyright
#iconv -f ISO88591 -t utf-8 $BASE/all/opt/IBM/ibmtts/doc/license.txt >> $doc/copyright

getChangelog $NAME $VERSION $doc/changelog
gzip -9n $doc/changelog
#buildpkg $NAME $VERSION "Voxin: 32 bits root filesystem" $dst

tar -C $dst -Jcf $PKGDIR/${NAME}_${VERSION}.txz .
