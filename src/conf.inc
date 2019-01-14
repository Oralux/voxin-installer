# The BASE variable should be set before including this file
#
VOXIN_VER=2.0

# TODO
ARCH=x86_64

#-- voxin-rfs32
# Buildroot
BR_VER=2017.02.9
BR_URL=https://buildroot.org/downloads/buildroot-${BR_VER}.tar.gz
# BR_PKG defines below

# files copied from Buildroot
RFS32_LIST="./etc/issue ./etc/os-release ./lib/ld-2.23.so ./lib/ld-linux.so.2 ./lib/libc-2.23.so ./lib/libc.so.6 ./lib/libdl-2.23.so ./lib/libdl.so.2 ./lib/libm-2.23.so ./lib/libm.so.6 ./lib/libpthread-2.23.so ./lib/libpthread.so.0 ./usr/share/doc/glibc*/copyright"

#-- voxin-libstdc++
LIBSTDCPP_ORIG_NAME=libstdc++2.10-glibc2.2
LIBSTDCPP_URL=http://archive.debian.org/debian/pool/main/g/gcc-2.95/${LIBSTDCPP_ORIG_NAME}_2.95.4-27_i386.deb

#-- libvoxin/voxind
LIBVOXIN_URL=https://github.com/Oralux/libvoxin/archive

# select the libvoxin sources (development or tag)
# - development (master branch)
LIBVOXIN_VERSION=
# - from the 1.3.3 tag instead (+sha256)
#LIBVOXIN_VERSION=1.3.3
#LIBVOXIN_SHA256=29a211d46f75ee1b8f3922e871554777a64815b1a4a2ff8f3c41e58012af2ea6

# Dependencies
DEP_LIST="make gcc wget fakeroot unzip emacs"

#####

PKGDIR=$BASE/build/packages
TMPDIR=$BASE/build/tmp
DWLDIR=$BASE/build/download
SRCDIR=$BASE/src
BR_PKG="$PKGDIR/buildroot_${BR_VER}.txz"
RFS_PKG="$PKGDIR/rfs_${VOXIN_VER}.txz"
RFSDIR=$TMPDIR/rfs
INSTALLDIR=$TMPDIR/voxin-installer_$VOXIN_VER
VOXINDIR=$RFSDIR/opt/oralux/voxin
RFS32=$VOXINDIR/rfs32
#RFS32_PKG="$PKGDIR/${NAME}.txz"
DOC=$VOXINDIR/usr/share/doc


# gnupg key used to sign packages
SIGNING_KEY=AED093E5

leave() {
	echo -e "$1" && exit $2
}


buildBuildroot() {
	local tarball="$BR_PKG"
	local config="$BASE/src/buildroot/config.buildroot-${BR_VER}"
	local workdir="$BASE/build/buildroot"
	local outputDir="$workdir/buildroot-${BR_VER}/output"
	# path of the 32 bits rootfilesystem built by buildroot.sh
	local targetDir="$outputDir/target"
	local GLIBC_VER=2.23
	local GLIBC_LICENSE="$outputDir/build/glibc-$GLIBC_VER/LICENSES"
	local archivep="$DWLDIR/$(basename $BR_URL)"
	local doc
	local free
	
	if [ -d "$workdir" ]; then
		echo "$workdir exists!"
		exit 0
	fi

	# check available storage space
	free=$(($(stat -f -c %a*%S .)/(1000000)))
	if [ $free -lt 3500 ]; then
		echo "Not enough free space: around 3.5 Go of available storage space needed"
		exit 0
	fi

	mkdir -p "$workdir"
	cd "$workdir"

	if [ ! -e "$archivep" ]; then
		wget -P "$DWLDIR" "$BR_URL"
	fi
	tar -xf "$archivep"
	cd buildroot-${BR_VER}
	cp "$config" .config

	make

	if [ -d "$targetDir" ]; then
		doc=$targetDir/usr/share/doc/glibc-$GLIBC_VER
		mkdir -p "$doc"
		cp $GLIBC_LICENSE $doc/copyright
		fakeroot bash -c "tar -C \"$targetDir\" -Jcf $tarball ."	
		echo $tarball
	else
		leave "Buildroot error" 1
	fi
}


getMinimalRFS32FromBuildroot() {
	if [ ! -f "$BR_PKG" ]; then
		echo "
Buildroot tarball not found: $BR_PKG
You may want to run build.sh -b
"	
		exit 1
	fi

	rm -rf "$RFSDIR"
	mkdir -p "$RFSDIR"

	mkdir -p $RFS32
	tar -C "$RFS32" -xf "$BR_PKG" --wildcards $RFS32_LIST
	find $RFS32 -type f -executable ! -name "ld-*" -exec chmod a-x {} \;
}


getOldLibstdc++() {
	local workdir="$TMPDIR/libstdc++"
	local pwd=$PWD
	local archivep="$DWLDIR/$(basename $LIBSTDCPP_URL)"

	rm -rf "$workdir"
	mkdir -p "$workdir"
	cd "$workdir"
	if [ ! -e "$archivep" ]; then
		wget -P "$DWLDIR" $LIBSTDCPP_URL
	fi
	ar -x "$archivep"
	tar -xf data.tar.gz
	cp -a usr "$RFS32"/
	cd "$pwd"
}


getLibvoxin() {
	local archive
	local archivep
	local dev
	local version
	local pwd=$PWD
	local workdir
	
	if [ -n "$LIBVOXIN_VERSION" ]; then
		unset dev
		version=$LIBVOXIN_VERSION
		archive=$version.tar.gz		
	else
		version=1.x.x-dev
		dev=1
		archive=master.zip
		[ -e "$DWLDIR"/$archive ] && mv "$DWLDIR"/$archive "$DWLDIR"/$archive.old
	fi
	archivep="$DWLDIR/$archive"
	
	unset missing
	for i in $DEP_LIST; do
		a=$(which $i) || missing="$missing $i"
	done
	
	[ -n "$missing" ] && leave "\nInstall please these binaries:\n$missing\n" 0
	
	workdir="$TMPDIR/libvoxin"
	rm -rf "$workdir"
	mkdir -p "$workdir"
	cd "$workdir"
	
	# download
	if [ ! -e "$archivep" ]; then
		wget -P "$DWLDIR" $LIBVOXIN_URL/$archive
	fi

	if [ -n "$dev" ]; then
		[ ! -d libvoxin-master ] && unzip "$archivep" || true
	else
		a=$(sha256sum "$archivep" | cut -f1 -d" ")
		if [ "$a" != "$LIBVOXIN_SHA256" ]; then
			echo "$archivep: sha256 mismatch !"
			exit 1
		fi
		[ ! -d libvoxin-$version ] && tar -zxf "$archivep" || true
	fi
	
	##
	
	if [ -n "$dev" ]; then
		cd libvoxin-master
	else
		cd libvoxin-$version
	fi
	./build.sh -r

	# libibmeci-fake_1.3.3.any.txz
	# libvoxin_1.3.3.x86_64.txz
	# voxin-pkg_1.3.3.any.txz

	tar -C "$RFSDIR" -xf build/$ARCH/release/libvoxin_*.$ARCH.txz
	cd "$pwd"	
}


buildReleaseTarball() {
	local dir="voxin-installer_$VOXIN_VER"
	local workdir="$TMPDIR/$dir"
	rm -rf "$workdir"
	mkdir -p "$workdir"	
	cp -a "$RFSDIR" "$workdir"
	cp -a "$SRCDIR/tools" "$workdir"
	fakeroot bash -c "tar -C \"$TMPDIR\" -Jcf \"$PKGDIR/$dir.$ARCH.txz\" $dir"
	echo "$PKGDIR/$dir.$ARCH.txz"
}

addFiles() {
	local files=$1
	if [ -f "$files" ]; then
		for i in $(cat "$files"); do
			tar -C "$RFSDIR" -xf $i
		done
	fi
}


buildInstaller() {
	local pwd=$PWD

	rm -rf "$INSTALLDIR"
	mkdir -p "$INSTALLDIR"
	cp -a src/installer/* "$INSTALLDIR"
	cd "$INSTALLDIR"

	emacs README.org --batch -f org-ascii-export-to-ascii --kill
	sed -i -e "s/%VERSION%/$VOXIN_VER/" -e '/^[[:space:]]*[=~_]\+$/d' README.txt 
	rm README.org

	cd po
	for i in de es fr; do
		./install_po.sh $i
	done


	cd "$pwd"
}
