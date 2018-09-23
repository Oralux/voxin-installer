#!/bin/bash -e

BASE=$(realpath $(dirname "$0"))

. ./conf.inc

PV=$MAJ.$MIN.$REV

#apt-get install debhelper

LIST="build-essential wget dpkg-dev fakeroot debhelper lintian"
dpkg -L $LIST &> /dev/null || apt-get install -y $LIST

getControl() {
	local name=$1
	local version=$2
	local size=$3
	local description=$4
	local control=$5
	local arch=$6
cat <<EOF>$control
Package: $name
Version: $version
Architecture: $arch
Maintainer: Gilles Casse <gcasse@oralux.org>
Section: sound
Priority: optional
Installed-Size: $size
Description: $description
 Voxin is an easily installable add-on which provides yet another
 text-to-speech to blind users of GNU/Linux.
EOF

}

getChangelog() {
	local name=$1
	local version=$2
	local file=$3
	local date=$(LANG=en_US.UTF-8 date -R)
	cat <<EOF>$file
$name ($version) UNRELEASED; urgency=medium

  * New upstream release: dedicated 32 bits rfs directory

 -- Gilles Casse <gcasse@oralux.org>  $date

$name (1.1.10-1) UNRELEASED; urgency=medium

  * New upstream release: don't replace xml entities with white spaces (case of "I'd ")

 -- Gilles Casse <gcasse@oralux.org>  Sun, 22 Jul 2018 17:41:56 +0100

$name (1.1.8-1) UNRELEASED; urgency=medium

  * New upstream release: convert xml predefined entities

 -- Gilles Casse <gcasse@oralux.org>  Sun, 1 Oct 2017 18:56:22 +0200

$name (1.1.7-1) UNRELEASED; urgency=medium

  * New upstream release: enable charset conversion with transliteration

 -- Gilles Casse <gcasse@oralux.org>  Mon, 17 Apr 2017 12:37:18 +0200

$name (1.1.6-1) UNRELEASED; urgency=medium

  * New upstream release: fix: converts text to the expected charset

 -- Gilles Casse <gcasse@oralux.org>  Sun, 16 Apr 2017 19:13:55 +0200

$name (1.1.5-1) UNRELEASED; urgency=medium

  * New upstream release: xml filter

 -- Gilles Casse <gcasse@oralux.org>  Sat, 1 Apr 2017 18:15:00 +0200

$name (1.1.4-1) UNRELEASED; urgency=medium

  * New upstream release: fix spurious spellings in the punctuation filter

 -- Gilles Casse <gcasse@oralux.org>  Sun, 26 Mar 2017 12:20:03 +0200

$name (1.1.2-1) UNRELEASED; urgency=medium

  * New upstream release.

 -- Gilles Casse <gcasse@oralux.org>  Sun, 9 Oct 2016 06:17:44 +0200

$name (1.0.0-1) UNRELEASED; urgency=medium

  * Initial release.

 -- Gilles Casse <gcasse@oralux.org>  Sat, 23 Jan 2016 11:29:06 +0100

EOF
	
}

getOverride() {
	local name=$1
	local dst=$2
	local label=$3
 	mkdir -p $dst/usr/share/lintian/overrides

# 	echo "\
##The $name package adds files to a global 32 bits rootfilesystem" > $dst/usr/share/lintian/overrides/$name
	rm -f $dst/usr/share/lintian/overrides/$name
 	for label in $label; do
 		echo "$name binary: $label" >> $dst/usr/share/lintian/overrides/$name
 	done
}


getCopyright() {
	local file=$1
	cat <<EOF>$file
Format: http://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Files: *
Copyright: 2018 Gilles Casse
License: LGPL-2.1+
 .
 This is free software; you can redistribute it and/or modify it under the
 terms of the GNU Leser General Public License as published by the Free
 Software Foundation; either version 2.1, or (at your option) any later
 version.
 .
 This software is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 .
 On Debian systems, you can find the GNU Lesser General Public License
 version 2.1 in the file /usr/share/common-licenses/LGPL-2.1.
EOF
	}


buildpkg() {
	local name=$1
	local version=$2
	local description=$3
	local dst=$4
	
	pushd "$dst"
	find . ! -path "./DEBIAN/*" -type f | xargs md5sum > DEBIAN/md5sums
	local size=$(du -s .|cut -f1)
	getControl $name $version $size "$description" DEBIAN/control "$ARCH"
	cd ..
	fakeroot dpkg-deb --build ${name} $PKGDIR
	popd
}


[ ! -d build ] && mkdir build
cd build
download
extract


cd libvoxin*
./build.sh -R
ARCHIVE_DIR=$PWD/build/x86_64/release
cd ..

mkdir -p "$PKGDIR"
declare -a NAMES=(libvoxin1 libvoxin1-dev voxind)
declare -a ARCHS=(amd64 all all)
declare -a OVERRIDES=("missing-depends-line symlink-should-be-absolute" "" "missing-depends-line symlink-should-be-absolute binary-without-manpage arch-independent-package-contains-binary-or-object binary-or-shlib-defines-rpath")
declare -a DESCRIPTIONS=("eases the integration of voxin on 64 bits architectures" "header file" "32 bits daemon linked to the ibmtts libraries")
MAX_NAME=${#NAMES[@]}
i=0
while [ "$i" -lt "$MAX_NAME" ]; do
	NAME=${NAMES[$i]}
	ARCH=${ARCHS[$i]}
	OVERRIDE=${OVERRIDES[$i]}
	DESCRIPTION=${DESCRIPTIONS[$i]}
	
	rm -rf "$TMPDIR" "$PKGDIR/${NAME}_${VERSION}_$ARCH.deb"
	mkdir -p "$TMPDIR"
	
	dst=$TMPDIR/$NAME	
	rfs32=$dst/$RFS32
	
	mkdir -p "$dst"/DEBIAN
	tar -C "$dst" -zxf $ARCHIVE_DIR/$NAME-$PV-*.tgz
	
	cd "$BASE"
	doc=$dst/usr/share/doc/$NAME
	mkdir -p "$doc"
	getCopyright $doc/copyright
	#iconv -f ISO88591 -t utf-8 $BASE/all/opt/IBM/ibmtts/doc/license.txt >> $doc/copyright
	
	getChangelog $NAME $VERSION $doc/changelog.Debian
	gzip -9n $doc/changelog.Debian
	[ -n "$OVERRIDE" ] && getOverride $NAME $dst "$OVERRIDE" || true
	buildpkg $NAME $VERSION "$DESCRIPTION" $dst
	
	lintian ${PKGDIR}/${NAME}_${VERSION}_${ARCH}.deb || true

	i=$((i+1))
done





