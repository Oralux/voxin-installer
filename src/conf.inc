# The BASE variable should be set before including this file
#
VOXIN_VER=2.0

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
DEP_LIST="make gcc wget fakeroot unzip emacs md5sum rsync bsdtar"

#####

PKGDIR=$BASE/build/packages
TMPDIR=$BASE/build/tmp
TEMPLATE=voxin-installer.XXXXXXXXXX
DWLDIR=$BASE/build/download
SRCDIR=$BASE/src
BR_PKG="$PKGDIR/buildroot_${BR_VER}.txz"
RFSDIR=$TMPDIR/rfs
INSTALLDIR=$TMPDIR/voxin-installer_$VOXIN_VER
VOXINDIR=$RFSDIR/opt/oralux/voxin
RFS32=$VOXINDIR/rfs32

#RFS32_PKG="$PKGDIR/${NAME}.txz"
DOC=$VOXINDIR/usr/share/doc
OLD_SD_VER=0.8.6-5voxin1
NEW_SD_VER=0.9
OLD_VOXIN_URL=https://voxin.oralux.net/update/voxin-update-1.15.tgz
OLD_VOXIN_SD_PATH=voxin-1.15/voxin-update-1.15/packages/ubuntu.17.04/speech-dispatcher-voxin_${OLD_SD_VER}_amd64.deb
OLD_VOXIN_SD_SUB_DIR=4

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
		wget -P "$DWLDIR" "$LIBSTDCPP_URL"
	fi
	ar -x "$archivep"
	tar -xf data.tar.gz
	cp -a usr "$RFS32"/
	cd "$pwd"
}

getDebArch() {
	local arch=$1
	case "$arch" in
		x86_64) arch=amd64;;
		*) leave "TODO: arch=$arch" 1;;
	esac
	echo $arch
}

# Build libvoxin from github and install the resulting deb and txz
# into the installer directory
# 
getLibvoxin() {
	[ "$#" != 1 ] && return 1
	local arch=$1
	local archive
	local archivep
	local dev
	local version
	local pwd=$PWD
	local workdir
	local installDir=$INSTALLDIR/packages/all

	[ ! -d "$installDir" ] && mkdir -p "$installDir"
		
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
		wget -P "$DWLDIR" "$LIBVOXIN_URL/$archive"
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

	# install the resulting build/$arch/release/libvoxin_$version.$archx.txz in the rfsdir
	local tarball=$(ls "$PWD"/build/"$arch"/release/libvoxin_*."$arch".txz)
	tar -C "$RFSDIR" -xf $tarball

	# build the final rfsdir tarball and install it in the installer tree
	tarball="$installDir"/rfs_$VOXIN_VER.$arch.txz
	fakeroot bash -c "tar -C \"$RFSDIR\" -Jcf $tarball ."	
	
	# build the libvoxin1 debian package and install it in the installer tree
	local description="Description: voxin pseudo package\n Voxin is an easily installable add-on which provides yet another\n text-to-speech to blind users of GNU/Linux.\n"
	tarball=$(ls "$PWD"/build/"$arch"/release/libvoxin-pkg_*.all.txz)
	local name=libvoxin1
	local depends=libc6
	debArch=$(getDebArch "$arch")
	installDir=$INSTALLDIR/packages/debian
	buildDebPackage "$tarball" "$installDir" "$name" "$version" "$debArch" "$depends" "$description" 

	installDir=$INSTALLDIR/packages/archlinux
	buildPacmanPackage "$tarball" "$installDir" "$name" "$version" "$arch" "$depends" "$description" "LGPL-2.1+" 
	
	cd "$pwd"	
}

# installs the speech-dispatcher-voxin debian package(s)
getSpeechDispatcherVoxin() {
	# Extracts speech-dispatcher-voxin from the voxin 1.15 update tarball
	# This package can be rebuild in following for example:
	# https://github.com/Oralux/speech-dispatcher-voxin-debian
	#
	[ "$#" != 1 ] && return 1
	local arch=$1
	local basename="$(basename $OLD_VOXIN_URL)"
	local archivep="$DWLDIR/$basename"
	local installDir=$INSTALLDIR/packages/debian
	local debArch=$(getDebArch "$arch")
	
	[ ! -d "$installDir" ] && mkdir -p "$installDir"
	
	if [ ! -e "$archivep" ]; then	
		wget -P "$DWLDIR" "$OLD_VOXIN_URL"
	fi
	tar --one-top-level="$installDir" --strip-components="$OLD_VOXIN_SD_SUB_DIR" -xf "$DWLDIR/$basename" "$OLD_VOXIN_SD_PATH"

	# speech-dispatcher-voxin 0.9: symlink to 0.8.6 (the ibmtts modules are currently the same)	
	ln -sf "$(basename $OLD_VOXIN_SD_PATH)" "$installDir"/speech-dispatcher-voxin_${NEW_SD_VER}_${debArch}.deb

	# build the archlinux package from the data archive
	local pacdir="$TMPDIR"/pac.d
	[ -e "$pacdir" ] && rm -rf "$pacdir"
	mkdir -p "$pacdir"
	
	pushd "$pacdir" 1>/dev/null
	ar -x "$installDir"/speech-dispatcher-voxin_${NEW_SD_VER}_${debArch}.deb data.tar.xz
	
	installDir=$INSTALLDIR/packages/archlinux
	buildPacmanPackage "$pacdir"/data.tar.xz "$installDir" speech-dispatcher-voxin "$OLD_SD_VER" "$arch" "libvoxin1" "Speech Dispatcher: voxin output module" "LGPL-2.1+" 

	# speech-dispatcher-voxin 0.9: symlink to 0.8
	ln -sf "speech-dispatcher-voxin-${OLD_SD_VER}-${arch}.pkg.tar.xz" "$installDir"/speech-dispatcher-voxin-${NEW_SD_VER}-${arch}.pkg.tar.xz
	popd 1>/dev/null	
}

buildReleaseTarball() {
	[ "$#" = 0 ] && return 1
	local arch=$1
	local files=$2
	local releaseDir=$PKGDIR/voxin-$VOXIN_VER
	local installer

	rm -rf "$releaseDir"
	mkdir -p "$releaseDir"

	# voxin-update
	installer="$releaseDir"/voxin-update-$VOXIN_VER
	mkdir "$installer" || return 1
	rsync -a "$INSTALLDIR"/ "$installer"
	
	fakeroot bash -c "tar -C \"$releaseDir\" -Jcf \"$installer.txz\" $installer"
	rm -rf "$installer"
	echo "$installer.txz"

	if [ -f "$files" ]; then
		local tarball
		local all
		local lang		
		for tarball in $(cat "$files"); do
			case "$tarball" in
				*voxin-viavoice-*txz)
					lang=$(echo "$tarball" | sed 's/.*-\(.*\)_.*/\1/')
					if [ "$lang" = "all" ]; then
						all=$tarball
					else
						installer="$releaseDir/voxin-$lang-$VOXIN_VER"
						mkdir "$installer"
						rsync -a "$INSTALLDIR"/ "$installer"
						cp -a "$tarball" "$all" "$installer"/packages/all
						fakeroot bash -c "tar -C \"$releaseDir\" -Jcf \"$installer.txz\" voxin-$lang-$VOXIN_VER"
						rm -rf "$installer"
						echo "$installer.txz"
					fi
					;;
				*) ;;
			esac
		done
	fi
}

buildInstallerDir() {
	local pwd=$PWD

	rm -rf "$INSTALLDIR"
	mkdir -p "$INSTALLDIR"
	rsync --exclude "*~" -a "$BASE"/src/installer/* "$INSTALLDIR"
	cd "$INSTALLDIR"

	emacs README.org --batch -f org-ascii-export-to-ascii --kill
	sed -e "s/%VERSION%/$VOXIN_VER/" -e '/^[[:space:]]*[=~_]\+$/d' README.txt > README
	rm README.org README.txt

	cd po
	for i in de es fr; do
		./install_po.sh $i
	done

	cd "$pwd"
}

buildDebPackage() {
	[ "$#" -lt 7 ] && return 1
	local tarball=$1
	local installDir=$2
	local name=$3
	local version=$4
	local arch=$5
	local depends=$6
	local description=$7
	local replaces=$8
		
	which md5sum > /dev/null || leave "install md5sum" 1

	local debdir="$TMPDIR"/deb.d
	[ -e "$debdir" ] && rm -rf "$debdir"
	mkdir -p "$debdir"/{DEBIAN,data}
	cd "$debdir"/data

	tar -xf "$tarball"
	find . -type f | xargs md5sum >> ../DEBIAN/md5sums
	local size=$(du -scb . | head -n1 | cut -f1)
	cd ..
	cat <<EOF>DEBIAN/control
Package: $name
Version: $version
Architecture: $arch
Maintainer: voxin-installer <contact@oralux.org>
Installed-Size: $size
Depends: $depends
Section: sound
EOF

	[ -n "$replaces" ] && echo "Replaces: $replaces" >> DEBIAN/control
	echo -e "$description" >> DEBIAN/control

	cp "$tarball" data.tar.xz
	echo 2.0 > debian-binary
	tar -C DEBIAN -Jcf control.tar.xz ./control ./md5sums

	[ ! -d "$installDir" ] && mkdir -p "$installDir"
	ar -r "$installDir"/${name}_${version}_${arch}.deb debian-binary control.tar.xz data.tar.xz 
}

buildPacmanPackage() {
	[ "$#" -lt 7 ] && return 1
	local tarball=$1
	local installDir=$2
	local name=$3
	local version=$4
	local arch=$5
	local depends=$6
	local description=$7
	local license=$8
	local replaces=$9
	local date=$(date --utc +%s)
	
	[ $# != 8 ] && return

	local pacdir="$TMPDIR"/pac.d
	[ -e "$pacdir" ] && rm -rf "$pacdir"
	mkdir -p "$pacdir"
	cd "$pacdir"

	tar --exclude=share -xf "$tarball"
	local size=$(du -scb . | head -n1 | cut -f1)

	cat <<EOF>.PKGINFO
pkgname = $name
pkgver = $version
pkgdesc = $description
url = https://voxin.oralux.net
builddate = $date
packager = voxin-installer <contact@oralux.org>
size = $size
arch = $arch
license = $license
depend = $depends
EOF

	[ -n "$replaces" ] && echo "replaces = $replaces" >> .PKGINFO

	[ ! -d "$installDir" ] && mkdir -p "$installDir"
	
	# Inspired by debtap: https://github.com/helixarch/debtap
	bsdtar -czf .MTREE --format=mtree --options='!all,use-set,type,uid,gid,mode,time,size,md5,sha256,link' * .PKGINFO .INSTALL 2> /dev/null
	fakeroot << EOF
tar -pcf "$installDir/$name-$version-$arch.pkg.tar" * .PKGINFO .INSTALL .MTREE 2> /dev/null
xz -z -9 "$installDir/$name-$version-$arch.pkg.tar"
EOF
	
}
