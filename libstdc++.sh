#!/bin/bash -xe

BASE=$(realpath $(dirname "$0"))

. ./conf.inc

VERSION=1.0-1
ORIG_NAME=libstdc++2.10-glibc2.2
NAME=voxin-$ORIG_NAME
URL=http://archive.debian.org/debian/pool/main/g/gcc-2.95/${ORIG_NAME}_2.95.4-27_i386.deb
PKGDIR=$BASE/build/packages
TMPDIR=$BASE/build/tmp

getControl() {
	local name=$1
	local version=$2
	local size=$3
	local description=$4
	local control=$5
cat <<EOF>$control
Package: $name
Version: $version
Architecture: all
Maintainer: Gilles Casse <gcasse@oralux.org>
Section: sound
Priority: optional
Installed-Size: $size
Description: $description
 The GNU stdc++ library
 NOTE: This is not a final release, but taken from the CVS gcc-2_95-branch
 (dated 2001-10-02).
 .
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.

EOF

}

getChangelog() {
	local name=$1
	local version=$2
	local file=$3
	local date=$(LANG=en_US.UTF-8 date -R)
	cat <<EOF>$file
$name ($version) UNRELEASED; urgency=medium

  * Installs in the rfs32 directory the original Debian package available at :
    $URL

 -- Gilles Casse <gcasse@oralux.org>  $date

EOF
	
}

getOverride() {
	local name=$1
	local dst=$2
	mkdir -p $dst/usr/share/lintian/overrides

#	echo "\
## The $name package adds files to a global 32 bits rootfilesystem used
#" > $dst/usr/share/lintian/overrides/$name
	rm -f $dst/usr/share/lintian/overrides/$name
	for label in package-contains-timestamped-gzip arch-independent-package-contains-binary-or-object unstripped-binary-or-object missing-depends-line new-package-should-close-itp-bug old-fsf-address-in-copyright-file shlib-with-executable-stack; do
		echo "$name binary: $label" >> $dst/usr/share/lintian/overrides/$name
	done
}

buildpkg() {
	local name="$1"
	local version="$2"
	local description="$3"
	local dst="$4"
	
	pushd "$dst"
	find . ! -path "./DEBIAN/*" -type f | xargs md5sum > DEBIAN/md5sums
	local size=$(du -s .|cut -f1)
	getControl $name $version $size "$description" DEBIAN/control
	cd ..
	fakeroot dpkg-deb --build ${name} $PKGDIR
	popd
}


rm -rf "$TMPDIR" "$PKGDIR/${NAME}_${VERSION}_all.deb"
mkdir -p "$PKGDIR" "$TMPDIR"
cd "$TMPDIR"
wget $URL

dst=$TMPDIR/$NAME	
rfs32=$dst/$RFS32
mkdir -p "$rfs32"/usr

cd $dst
dpkg-deb -e ../$(basename $URL)
dpkg-deb -x ../$(basename $URL) .
rm -rf DEBIAN/*
mv usr $rfs32/

doc=$dst/usr/share/doc/$NAME
mkdir -p "$doc"
getChangelog $NAME $VERSION $doc/changelog.Debian
gzip -9n $doc/changelog.Debian
mv $rfs32/usr/share/doc/$ORIG_NAME/copyright $doc
getOverride $NAME $dst
buildpkg $NAME $VERSION "installs $ORIG_NAME to the 32 bits root filesystem" $dst


lintian $PKGDIR/${NAME}_${VERSION}_all.deb
