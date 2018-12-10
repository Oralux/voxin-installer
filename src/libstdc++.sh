#!/bin/bash -xe

BASE=$(realpath $(dirname "$0")/..)
. $BASE/src/conf.inc

VERSION=1.0-1
LIBSTDCPP_ORIG_NAME=libstdc++2.10-glibc2.2
NAME=voxin-libstdc++

getChangelog() {
	local name=$1
	local version=$2
	local file=$3
	local date=$(LANG=en_US.UTF-8 date -R)
	cat <<EOF>$file
$name ($version) UNRELEASED; urgency=medium

  * Installs in the rfs32 directory the original Debian package available at :
    $LIBSTDCPP_URL
    Described as:
    "The GNU stdc++ library
    NOTE: This is not a final release, but taken from the CVS gcc-2_95-branch
    (dated 2001-10-02).
 
    This package contains an additional runtime library for C++ programs
    built with the GNU compiler."

 -- Gilles Casse <gcasse@oralux.org>  $date

EOF
	
}

rm -rf "$TMPDIR" "$PKGDIR/${NAME}_${VERSION}_all.deb"
mkdir -p "$PKGDIR" "$TMPDIR"
cd "$TMPDIR"
wget $LIBSTDCPP_URL

dst=$TMPDIR/$NAME	
rfs32=$dst/$RFS32
mkdir -p "$rfs32"/usr

cd $dst
dpkg-deb -e ../$(basename $LIBSTDCPP_URL)
dpkg-deb -x ../$(basename $LIBSTDCPP_URL) .
rm -rf DEBIAN/*
mv usr $rfs32/

doc=$(dirname $rfs32)/share/doc/$NAME
mkdir -p "$doc"
getChangelog $NAME $VERSION $doc/changelog.Debian
gzip -9n $doc/changelog.Debian
mv $rfs32/usr/share/doc/$LIBSTDCPP_ORIG_NAME/copyright $doc

