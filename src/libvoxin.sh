#!/bin/bash -xe

BASE=$(realpath $(dirname "$0")/..)
. $BASE/src/conf.inc

if [ -n "$LIBVOXIN_VERSION" ]; then
	unset DEV
	ARCHIVE=$LIBVOXIN_VERSION.tar.gz
else
	LIBVOXIN_VERSION=1.x.x-dev
	DEV=1
	ARCHIVE=master.zip
fi

VERSION=$LIBVOXIN_VERSION-1

LIST="build-essential wget fakeroot unzip"
for i in $LIST; do
	debianIsPackageInstalled $i || leave "\nInstall packages:\n$LIST\n" 0
done

download() {
    if [ ! -e "$ARCHIVE" ]; then
	   	wget $LIBVOXIN_URL/$ARCHIVE
    fi
	if [ -z "$DEV" ]; then
		a=$(sha256sum "$ARCHIVE" | cut -f1 -d" ")
		if [ "$a" != "$LIBVOXIN_SHA256" ]; then
			echo "$ARCHIVE: sha256 mismatch !"
			exit 1
		fi
		[ ! -d libvoxin-$LIBVOXIN_VERSION ] && tar -zxf "$ARCHIVE" || true
	else
		[ ! -d libvoxin-master ] && unzip "$ARCHIVE" || true
	fi
}


getCopyright() {
	local file=$1
	cat <<EOF>$file
Copyright: 2018 Gilles Casse
License: LGPL-2.1+
 
This is free software; you can redistribute it and/or modify it under the
terms of the GNU Leser General Public License as published by the Free
Software Foundation; either version 2.1, or (at your option) any later
version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

On Debian systems, you can find the GNU Lesser General Public License
version 2.1 in the file /usr/share/common-licenses/LGPL-2.1.
EOF
}

getScripts() {
	local name=$1
	local version=$2
	local dst=$3

	local maj=${version%%.*}
	local dir=$dst/DEBIAN

	[ "$name" != "libvoxin1" ] && return

	cat<<EOF>"$dir"/preinst
#!/bin/sh

set -e

case "\$1" in
	 install|upgrade)
		# remove any $name.so file 
		if [ "\$DPKG_MAINTSCRIPT_ARCH" = amd64 ]; then
		   ARCHDIR=/usr/lib/x86_64-linux-gnu
		else
			ARCHDIR=/usr/lib/i386-linux-gnu
		fi
	    rm -f \$ARCHDIR/libvoxin.so*
        ;;
     *) ;;
esac

exit 0
EOF

	chmod 755 "$dir"/preinst

}


[ ! -d build ] && mkdir build
cd build
download
if [ "$LIBVOXIN_VERSION" = "1.x.x-dev" ]; then
	cd libvoxin-master
else
	cd libvoxin-$LIBVOXIN_VERSION
fi
./build.sh -R
ARCHIVE_DIR=$PWD/build/x86_64/release
cd ..

mkdir -p "$PKGDIR"
declare -a NAMES=(libvoxin1 libvoxin1-dev voxind voxin-say)
declare -a ARCHS=(amd64     all           all    amd64)
declare -a DESCRIPTIONS=("eases the integration of voxin on 64 bits architectures" "header file" "32 bits daemon linked to the ibmtts libraries" "Converts text to speech written to the standard output or the supplied file")
MAX_NAME=${#NAMES[@]}
i=0
while [ "$i" -lt "$MAX_NAME" ]; do
	NAME=${NAMES[$i]}
	ARCH=${ARCHS[$i]}
	DESCRIPTION=${DESCRIPTIONS[$i]}
	
	rm -rf "$TMPDIR" "$PKGDIR/${NAME}_${VERSION}_$ARCH.deb"
	mkdir -p "$TMPDIR"
	
	dst=$TMPDIR/$NAME	
	rfs32=$dst/$RFS32
	
	mkdir -p "$dst"/DEBIAN
	CPU=$ARCH
	[ "$ARCH" = amd64 ] && CPU=x86_64 
	tar -C "$dst" -zxf $ARCHIVE_DIR/$NAME-*$CPU.tgz
	
	cd "$BASE"
	doc=$(dirname $rfs32)/share/doc/$NAME
	mkdir -p "$doc"
	getCopyright "$doc/copyright"
	#iconv -f ISO88591 -t utf-8 $BASE/all/opt/IBM/ibmtts/doc/license.txt >> $doc/copyright
	
#	getScripts "$NAME" "$LIBVOXIN_VERSION" "$dst"
#	buildpkg "$NAME" "$VERSION" "$DESCRIPTION" "$dst"
	fakeroot bash -c "tar -C \"$dst\" -Jcf \"$PKGDIR/${NAME}_${VERSION}.txz\" ."	
	i=$((i+1))
done


cat <<EOF>$PKGDIR/README

The libvoxin packages were built using libvoxin-debian:
https://github.com/Oralux/libvoxin-debian

EOF

