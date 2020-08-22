# The BASE variable should be set before including this file
#
VOXIN_VERSION=3.1rc3

VOXIN_INSTALLER_GIT_URL=https://github.com/Oralux/voxin-installer.git
VOXIN_INSTALLER_TAG=$VOXIN_VERSION

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
#LIBVOXIN_VERSION=
# - from the last tag instead (+sha512)
LIBVOXIN_VERSION=1.5.0
LIBVOXIN_SHA512=b527f5bc467a169eccc9d2561fba6ee8aefd2f1eaf196b0914c0b1142916afc12ae1e1d04edc77d4687255479af27d6e80f8d1d5a1ef95415de0f3fe9385bb99

# Previous speech-dispatcher voxin modules
# These binaries have been built with a previous release of
# voxin-installer (git tag stored in sources.ini inside the tarball)
PREV_SPEECHD_VOXIN_URL=http://soft.oralux.org/speech-dispatcher/bin
PREV_SPEECHD_VOXIN_ARCHIVE=sd_voxin_0.7.1_0.8.8.x86_64.txz
PREV_SPEECHD_VOXIN_SHA512=9080513ca9e2387dbebba6b45bf9a7c43932e4cc7042601658f516e9eefd9d9317b34424cff22eff83bf85fa7d116293116556ff26b9e094f653edb6869a020b

# VMX86_64: VM where voxin-installer is built for x86_64 architecture
# This VM is currently used to build libvoxin on a Debian 8 environment
# (mostly due to puncfilter and its libstdc++ dependency)
# 
VMX86_64=user1@192.168.122.218
VMVOXDIR=/home/user1/VOXIN
VMLIBVOXDIR=$VMVOXDIR/LIBVOXIN

# VMX86: VM where voxin-installer is built for x86 architecture
# This VM is currently used to build on a 64 bits machine the
# speech-dispatcher module for x86
# 
VMX86=user1@192.168.122.174

# Dependencies
# # libstdc++-4.9-dev:i386 (hard coded in function below)
#DEP_PACKAGES="alien bsdtar build-essential fakeroot rpm rsync build-essential binutils-multiarch libc6-dev-i386 gcc-multilib g++-multilib patchelf"
DEP_PACKAGES="alien bsdtar build-essential fakeroot rpm rsync"
# Debian9: speech-dispatcher dependencies
# obtained with apt-get build-dep speech-dispatcher

DEP_PACKAGES_X86="alien bsdtar rpm build-essential flite1-dev intltool libao-dev libasound2-dev libaudio-dev libdotconf-dev libespeak-dev libflac-dev libglib2.0-dev libogg-dev libpulse-dev libsndfile1-dev libvorbis-dev libxau-dev pkg-config tex-common x11proto-core-dev xorg-sgml-doctools zlib1g-dev dh-autoreconf libltdl-dev libtool"

# DEP_PACKAGES_SPEECHD="espeak-data flite1-dev intltool libao-dev libasound2-dev libaudio-dev \
#   libdotconf-dev libespeak-dev libespeak1 libflac-dev \
#   libglib2.0-dev libogg-dev \
DEP_PACKAGES_SPEECHD="espeak-data flite1-dev intltool libao-dev libasound2-dev libaudio-dev \
  libdotconf-dev libespeak-dev libespeak1 libflac-dev \
  libglib2.0-dev libogg-dev libpcre3-dev \
  libportaudio2 libpulse-dev libsndfile1-dev libtext-unidecode-perl \
  libvorbis-dev libxau-dev libxml-libxml-perl libxml-namespacesupport-perl \
  libxml-sax-base-perl libxml-sax-perl pkg-config tex-common texinfo \
  x11proto-core-dev xorg-sgml-doctools zlib1g-dev \
  dh-autoreconf libltdl-dev libtool \
"
# If host more recent than Debian 8
DEP_PACKAGES_SPEECHD_OTHER="libespeak-ng-dev libpcre16-3 libpcre32-3 libpcrecpp0v5"

#####

PKGDIR=$BASE/build/packages
TMPDIR=$BASE/build/tmp
TEMPLATE=voxin-installer.XXXXXXXXXX
DWLDIR=$BASE/build/download
SRCDIR=$BASE/src
BR_PKG="$PKGDIR/buildroot_${BR_VER}.txz"
RFSDIR=$TMPDIR/rfs
INSTALLDIR=$TMPDIR/voxin-installer_$VOXIN_VERSION
VOXINDIR=$RFSDIR/opt/oralux/voxin
RFS32=$VOXINDIR/rfs32
BUILDPKG=$TMPDIR/buildPackage
DWLX86=$DWLDIR/x86

#RFS32_PKG="$PKGDIR/${NAME}.txz"
DOC=$VOXINDIR/usr/share/doc

# gnupg key used to sign packages
SIGNING_KEY=AED093E5

# versions of the speech-dispatcher voxin module which can be built by this script.
# the older modules are supplied by an archive (sd_voxin_0.7.1_0.8.8.x86_64.tgz)
#
# first version == last version
SPEECHD_VOXIN_ALL_VERSIONS="0.9.1 0.9.0"

getArch() {
    ARCH=$(uname -m)
    case "$ARCH" in
        x86|i386|i586|i686) ARCH=i686;;
        armv7l) ARCH=armhf;;
    esac
}

# set the variables describing the speech-dispatcher voxin archive
# (filename, sha sum)
setSpeechdVoxinVar() {
    local version=$1
    SPEECHD_VOXIN_ARCHIVE_URL=https://github.com/Oralux/speechd/archive
    SPEECHD_VOXIN_GIT_URL=https://github.com/Oralux/speechd.git
    SPEECHD_VOXIN_TAG=voxin3rc4
    SPEECHD_SD_MODULE_COPYRIGHT_NB_LINES=25

    unset SPEECHD_VOXIN_ARCHIVE SPEECHD_VOXIN_SHA512
    case $version in
	0.9.1)	    
	    SPEECHD_VOXIN_ARCHIVE=$version-$SPEECHD_VOXIN_TAG.tar.gz
	    SPEECHD_VOXIN_SHA512=a0c46c8bed6244cf6c7a0577ab4f479944f5cd7d96e304aa0f0415c2b2e3f0131831d18c60013432e069035132d29118f76d1416e3d0b44d1d1a4cd94b14cbd0
	    ;;
	0.9.0)	    
	    SPEECHD_VOXIN_ARCHIVE=$version-$SPEECHD_VOXIN_TAG.tar.gz
	    SPEECHD_VOXIN_SHA512=7a64ee3940a720aa35cb3b1bc066f1b49ba328693342d6eb24c1183fb64a1a7786798e54a786a2813e6136d6df8a5c50887f4d08b4d3fcca4906cd20942868b3
	    ;;
	master)
	    SPEECHD_VOXIN_ARCHIVE=master.zip
	    ;;
	*)
	    leave "unexpected speechd voxin version ($version) 1"
	    ;;
    esac
}

leave() {
	echo -e "$1" && exit $2
}

isPackageInstalled() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local deb=$1
	grep -A1 "Package: $deb$" /var/lib/dpkg/status | grep "Status: install ok installed" &> /dev/null
}


checkForeignArch() {
return
	# [ "$(uname -m)" != x86_64 ] && return
	# ( dpkg --print-foreign-architectures | grep i386 ) \
	# 	|| leave "i386 foreign arch needed; please type as root: dpkg --add-architecture i386 && apt-get update" 1
}

checkDep() {
	local missing
	local i

	[ ! -e /var/lib/dpkg/status ] && leave "this script expects a Debian based distro (to check dependencies)" 1

	local dep="$DEP_PACKAGES"
	if [ "$(uname -m)" = i686 ]; then
	    dep="$DEP_PACKAGES_X86"
	fi
	# if host more recent than Debian 8 complete dependencies
	local id=$(awk -F= '/^ID=/{print $2}' /etc/os-release)
	local version_id=$(awk -F'"' '/VERSION_ID/{print $2}' /etc/os-release)
	if [ "$id" != debian ] || [ "$version_id" != 8 ]; then
		DEP_PACKAGES_SPEECHD=$(echo $DEP_PACKAGES_SPEECHD $DEP_PACKAGES_SPEECHD_OTHER)
	fi
	for i in $dep $DEP_PACKAGES_SPEECHD; do
	    isPackageInstalled $i || missing="$missing $i"
	done


	# i=libstdc++-4.9-dev:i386
	# dpkg -L $i 2>/dev/null || missing="$missing $i"

	
	[ -n "$missing" ] && leave "install these required dependencies:\n$missing" 1
}

init() {
	local i
	for i in "$TMPDIR" "$BUILDPKG" "$PKGDIR"/voxin* "$PKGDIR"/rfs* "$RFS32"; do
		rm -rf "$i"
	done
	mkdir -p "$PKGDIR" "$DWLDIR" "$BUILDPKG"/{rfs,build}
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
		if [ ! -e "$archivep" ]; then
			leave "$archivep not found" 1
		fi
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
		if [ ! -e "$archivep" ]; then
			leave "$archivep not found" 1
		fi
	fi
	ar -x "$archivep"
	tar -xf data.tar.gz
	cp -a usr "$RFS32"/
	cd "$pwd"
}

getPreviousSpeechdModules() {
	local dwldir="$DWLDIR/previous_speechd_voxin"
	local archivep="$dwldir/$PREV_SPEECHD_VOXIN_ARCHIVE"
	local installDir=$INSTALLDIR/packages/all
	local a
	
	if [ ! -e "$archivep" ]; then
	    mkdir -p "$dwldir"
	    wget -P "$dwldir" "$PREV_SPEECHD_VOXIN_URL/$PREV_SPEECHD_VOXIN_ARCHIVE"
	    if [ ! -e "$archivep" ]; then
		leave "$archivep not found" 1
	    fi
	fi

	a=$(sha512sum "$archivep" | cut -f1 -d" ")
	[ "$a" != "$PREV_SPEECHD_VOXIN_SHA512" ] && leave "$archivep: sha512 mismatch !" 1
	
	[ ! -d "$installDir" ] && mkdir -p "$installDir"	
	cp -a "$archivep" "$installDir"
}

getDebArch() {
	local arch=$1
	case "$arch" in
		x86_64) arch=amd64;;
		arm*) ;;
		*) arch=i386;;
	esac
	echo $arch
}

# Build libvoxin, install the resulting tarball into the local rfs,
# create the symlinks for the voxin package
# Build a fake libibmeci.so for test
# getLibvoxinRes: directory of the fake lib
getLibvoxin() {
	[ "$#" = 0 ] && return 1
	local arch=$1
	local archive
	local archivep
	local dev
	local version
	local pwd=$PWD
	local workdir=$TMPDIR/libvoxin
	local installDir=$INSTALLDIR/packages/all
	unset getLibvoxinRes
	
	[ ! -d "$installDir" ] && mkdir -p "$installDir"
		
	if [ -n "$LIBVOXIN_VERSION" ]; then
		unset dev
		version=$LIBVOXIN_VERSION
		archive=$version.tar.gz		
	else
		version=1.x.x
		dev=1
		archive=master.zip
		[ -e "$DWLDIR"/$archive ] && mv "$DWLDIR"/$archive "$DWLDIR"/$archive.old
	fi
	archivep="$DWLDIR/$archive"
	
	rm -rf "$workdir"
	mkdir -p "$workdir"
	cd "$workdir"
	
	# download
	if [ ! -e "$archivep" ]; then
		wget -P "$DWLDIR" "$LIBVOXIN_URL/$archive"
		if [ ! -e "$archivep" ]; then
			leave "$archivep not found" 1
		fi
	fi

	if [ -n "$dev" ]; then
		[ ! -d libvoxin-master ] && unzip "$archivep" || true
	else
		local a=$(sha512sum "$archivep" | cut -f1 -d" ")
		if [ "$a" != "$LIBVOXIN_SHA512" ]; then
			echo "$archivep: sha512 mismatch !"
			exit 1
		fi
		[ ! -d libvoxin-$version ] && tar -zxf "$archivep" || true
	fi	
	##
	
	if [ -n "$dev" ]; then
		cd libvoxin-master || leave "Can't cd to libvoxin-master" 1
	else
		cd libvoxin-$version || leave "Can't cd to libvoxin-$version" 1
	fi
	./build.sh -m "$arch" -r

	# install the resulting build/$arch/release/libvoxin_$version.$archx.txz in the rfsdir
	[ -d "$RFSDIR" ] || mkdir -p "$RFSDIR"
	local tarball=$(ls "$PWD"/build/"$arch"/release/libvoxin_*."$arch".txz)
	tar -C "$RFSDIR" -xf "$tarball" || leave "Can't uncompress libvoxin tarball ($tarball)" 1

	# create symlinks
	tarball=$(ls "$PWD"/build/"$arch"/release/libvoxin-pkg_*.all.txz)
	tar -C "$BUILDPKG/rfs" -xf "$tarball" || leave "Can't uncompress libvoxin-pkg tarball ($tarball)" 1

	getLibvoxinRes=$PWD/build/$arch/rfs/usr/lib
	
	cd "$pwd"
	return 0
}

# Build speech-dispatcher from github, install the resulting binaries
# into the local rfs, create the symlinks for the voxin package
getSpeechDispatcherVoxin() {
	[ "$#" -lt 2 ] && return 1
	local arch=$1
	local libvoxinDir=$2
	local version=$3
	local archive
	local archivep
	local dir
	local pwd=$PWD
	local workdir=$TMPDIR/speechd_voxin
	local installDir=$INSTALLDIR/packages/all
	local last=0
	
	[ ! -d "$installDir" ] && mkdir -p "$installDir"

	[ "$#" = "4" ] && [ "$4" = "last" ] && last=1

	setSpeechdVoxinVar "$version"
	archive=$SPEECHD_VOXIN_ARCHIVE
	sha512=$SPEECHD_VOXIN_SHA512
	archivep="$DWLDIR/$archive"
	
	rm -rf "$workdir"
	mkdir -p "$workdir"/{build,rfs,deb,pac}
	cd "$workdir/build"
	
	# download
	if [ "$version" = master ]; then
	    [ -e "$DWLDIR"/$archive ] && mv "$DWLDIR"/$archive "$DWLDIR"/$archive.old
	fi
	
	if [ ! -e "$archivep" ]; then
	    wget -P "$DWLDIR" "$SPEECHD_VOXIN_ARCHIVE_URL/$archive"
	    if [ ! -e "$archivep" ]; then
		leave "$archivep not found" 1
	    fi
	fi

	if [ -n "$sha512" ]; then
	    local a=$(sha512sum "$archivep" | cut -f1 -d" ")
	    [ "$a" != "$sha512" ] && leave "$archivep: sha512 mismatch !" 1
	fi

	if [ ! -d speechd_voxin-$version ]; then
	    if [ "${archivep##*.}" = zip ]; then
		unzip "$archivep" || true
		dir=speechd-${archive%.zip}
	    else
		pwd
		tar -xf "$archivep" || true
		dir=speechd-${archive%.tar.gz}
	    fi
	fi	
	
	cd $dir || leave "Can't cd to speechd-$version" 1
	   
	# # Patches needed to build speech-dispatcher on Debian 8
	# case "$version" in
	#     "$SPEECHD_VOXIN_VERSION_0_9_1")
	# 	patch -p1 < $SRCDIR/patch/sd-0.9-gettext-version.patch
	# 	patch -p1 < $SRCDIR/patch/sd-0.9-force-modif-acceptance.patch
	# 	;;
	#     "$SPEECHD_VOXIN_VERSION_0_9_0")
	# 	patch -p1 < $SRCDIR/patch/sd-0.9-gettext-version.patch
	# 	patch -p1 < $SRCDIR/patch/sd-0.9-force-modif-acceptance.patch
	# 	;;
	#     "$SPEECHD_VOXIN_VERSION_0_8")	
	# 	patch -p1 < $SRCDIR/patch/sd-0.8-fix-doc-from-debian-8-src.patch	    
	# 	;;
	#     "$SPEECHD_VOXIN_VERSION_0_7_1")	
	# 	patch -p1 < $SRCDIR/patch/sd-0.7.1-fix-doc-from-debian-8-src.patch	    
	# 	;;
	# esac
	./build.sh
	# if [ "$SPEECHD_VOXIN_VERSION_0_8" = "$version" ]; then
	#     unset GTHREAD_LIBS
	# else
	#     # pthread instead of gthread which is not always installed by default (seen in openSUSE Tumbleweed-i586 20190220)
	#     GTHREAD_LIBS=-lpthread
	# fi	    
#	GTHREAD_LIBS=$GTHREAD_LIB CFLAGS="-I$libvoxinDir/../include/voxin" LDFLAGS="-L$libvoxinDir" ./configure --prefix="/usr" --with-voxin
	CFLAGS="-I$libvoxinDir/../include/voxin" LDFLAGS="-L$libvoxinDir" ./configure --prefix="/usr"
	make || return 1
	
	# install
	local modulesDir="$VOXINDIR"/lib/speech-dispatcher-modules/$version
	local confDir="$VOXINDIR"/share/speech-dispatcher/$version
	local docDir="$VOXINDIR"/share/doc/speech-dispatcher-voxin/$version
	mkdir -p "$modulesDir" "$confDir" "$docDir"
	local module=src/modules/sd_voxin
	[ -e src/modules/.libs/sd_voxin ] && module=src/modules/.libs/sd_voxin
	# if [ "$SPEECHD_VOXIN_VERSION_0_7_1" = "$version" ]; then
	#     # CentOS7: rpath to libsdaudio
	#     patchelf --set-rpath /usr/lib64/speech-dispatcher "$module"
	# fi	    
	cp "$module" "$modulesDir" || return 1
	cp ./config/modules/voxin.conf "$confDir" || return 1
	if [ "$last" = "1" ]; then
	    ln -s "$version" ${modulesDir%[/]*}/last
	    ln -s "$version" ${confDir%[/]*}/last
	fi
	head -n $SPEECHD_SD_MODULE_COPYRIGHT_NB_LINES src/modules/voxin.c > "$docDir"/copyright
	cat<<EOF>"$docDir"/sources.ini
[archive]
url=$SPEECHD_VOXIN_ARCHIVE_URL/$SPEECHD_VOXIN_ARCHIVE
sha512=$SPEECHD_VOXIN_SHA512

[git]
url=$SPEECHD_VOXIN_GIT_URL
tag=$SPEECHD_VOXIN_TAG
EOF
	
	cd "$pwd"
	return 0
}

buildVoxinModule() {
	local pwd="$PWD"
	local modulesDir="$VOXINDIR"/lib/speech-dispatcher-modules
	cd "$SRCDIR"/speechd-voxin
	make clean
	make
	DESTDIR="$modulesDir" make install

	# create symlinks
	cd "$BUILDPKG/rfs"
	mkdir -p usr/{lib/speech-dispatcher-modules,share/speech-dispatcher/conf/modules} etc/speech-dispatcher/modules

	# Voxin module:
	# The speech-dispatcher module installed by the voxin package is just a
	# symbolic link to a module selector:
	# /usr/lib/speech-dispatcher-modules/sd_voxin
	# points to
	# /opt/oralux/voxin/lib/speech-dispatcher-modules/sd_voxin
	#
	# This module selector determines the version of speech-dispatcher using:
	# /usr/bin/speech-dispatcher -v
	# and launches the corresponding voxin module available under:
	# /opt/oralux/voxin/lib/speech-dispatcher-modules:
	#   0.9.1/sd_voxin
	#   0.9.0/sd_voxin
	#
	# If the module selector can't determine the speech-dispatcher
	# version (e.g. if speech-dispatcher has been built from git)
	# it will attempt to launch the most recent version of sd_voxin.
	#
	# The current module configuration is a symlink from:
	# /etc/speech-dispatcher/modules/voxin.conf
	# to 
	# /opt/oralux/voxin/share/speech-dispatcher/conf/voxin.conf
	# 
	# The default configuration files are stored under
	# /opt/oralux/voxin/share/speech-dispatcher/conf/
	#   0.9.1/voxin.conf
	#   0.9.0/voxin.conf
	#
	ln -s /opt/oralux/voxin/lib/speech-dispatcher-modules/sd_voxin usr/lib/speech-dispatcher-modules/sd_voxin
	ln -s /opt/oralux/voxin/share/speech-dispatcher/voxin.conf usr/share/speech-dispatcher/conf/modules/voxin.conf
	ln -s /opt/oralux/voxin/share/speech-dispatcher/voxin.conf etc/speech-dispatcher/modules/voxin.conf

	cd "$pwd"
}

# install the README and LICENSE files
# create the symlinks for the voxin package
getVoxinDoc() {
	local pwd=$PWD

	cp $INSTALLDIR/README $INSTALLDIR/LICENSE "$VOXINDIR"

	cd "$BUILDPKG/rfs"
	mkdir -p usr/share/doc/voxin
	ln -s /opt/oralux/voxin/LICENSE usr/share/doc/voxin/
	ln -s /opt/oralux/voxin/README usr/share/doc/voxin/
	
	cd "$pwd"
	return 0
}

buildPackage() {
	[ $# != 1 ] && return 1
	local arch=$1
	local tarball="$BUILDPKG"/build/voxin_$VOXIN_VERSION.all.txz
	local rfsdir=$BUILDPKG/rfs
	fakeroot bash -c "\
tar -C \"$rfsdir\" \
	   -Jcf \"$tarball\" ." leave "can't create tarball into $rfsdir" 1

	# package voxin for various package managers
	local title="Voxin pseudo package"
	local description="Voxin is an easily installable add-on which provides yet another text-to-speech to blind users of GNU/Linux. This package provides symbolic links to voxin."

	local name=voxin
	local depends=libc6
	local replaces

	# debian
	local debArch=$(getDebArch "$arch")
	replaces="libvoxin1, libvoxin1-dev, voxind, speech-dispatcher-voxin, speech-dispatcher-ibmtts"
	installDir=$INSTALLDIR/packages/deb
	buildDebPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$debArch" "$depends" "$title" "$description" "$replaces"

	# gentoo
	local depends=glibc
	replaces="libvoxin, voxind, speech-dispatcher-voxin, speech-dispatcher-ibmtts"
	installDir=$INSTALLDIR/packages/gentoo
	buildGentooPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# pacman
	local depends=glibc
	replaces="libvoxin, voxind, speech-dispatcher-voxin, speech-dispatcher-ibmtts"
	installDir=$INSTALLDIR/packages/archlinux
	buildPacmanPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# rpm
	replaces="libvoxin, libvoxin-debuginfo, libvoxin-devel, voxind, voxind-debuginfo, speech-dispatcher-voxin, speech-dispatcher-ibmtts"
	installDir=$INSTALLDIR/packages/rpm
	buildRpmPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces"
	
	# slackware
	installDir=$INSTALLDIR/packages/slackware
	buildSlackwarePackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+"
}

getLangFromViavoiceTarball() {
	local lang=$(echo "$tarball" | sed 's/.*voxin-viavoice-\(.*\)_.*/\1/')
	echo "$lang"
}

# build the voice tarball for a specific release
# voice: update-ve,...
# tarball: *voxind*,...
# all: *voxin-nve-all*,...
buildVoiceTarball() {
	local voice=$1
	local tarball=$2
	local all=$3
	local releaseDir=$PKGDIR/voxin-$VOXIN_VERSION
	local installDir=$INSTALLDIR/packages/all
	local installerDir="$releaseDir/voxin-$VOXIN_VERSION/voxin-$voice-$VOXIN_VERSION"
	local installerTar="$releaseDir/voxin-$voice-$VOXIN_VERSION.tgz"

	mkdir -p "$installerDir" || return 1
	rsync -a "$INSTALLDIR"/ "$installerDir"
	cp -a "$tarball" "$all" "$installerDir"/packages/all
	fakeroot bash -c "tar -C \"$releaseDir\" -zcf \"$installerTar\" voxin-$VOXIN_VERSION" || return 1
	rm -rf "$releaseDir/voxin-$VOXIN_VERSION"
	echo "$installerTar"	
}

buildReleaseTarball() {
	[ $# != 3 ] && return 1
	local files=$1
	local arch=$2
	local withTTS=$3
	local releaseDir=$PKGDIR/voxin-$VOXIN_VERSION
	# prefixed by withTTS to prevent deletion by next init() call
	local releaseTarball=$PKGDIR/${withTTS}_voxin-$VOXIN_VERSION.tgz
	local installerDir
	local installerTar
	local installDir
	local tarball
	local pwd=$PWD
	local exclude
	local docDir="$VOXINDIR"/share/doc/voxin-installer
	[ ! -d "$docDir" ] && mkdir -p "$docDir"
	cat<<EOF>"$docDir"/sources.ini
[git]
url=$VOXIN_INSTALLER_GIT_URL
tag=$VOXIN_INSTALLER_TAG
EOF

	# build the final rfsdir tarball and install it in the installer tree
	installDir=$INSTALLDIR/packages/all
	tarball=$installDir/rfs_$VOXIN_VERSION.all.txz
	[ ! -d "$installDir" ] && mkdir -p "$installDir"	
	fakeroot bash -c "tar -C \"$RFSDIR\" --exclude ./opt/oralux/voxin/bin --exclude ./opt/oralux/voxin/lib -Jcf $tarball ."	
	# make more visible the minimal rfs tarball, used by the external libvoxin project
	cp $tarball $PKGDIR

	installDir=$INSTALLDIR/packages/all
	tarball=$installDir/rfs_$VOXIN_VERSION.$arch.txz
	[ ! -d "$installDir" ] && mkdir -p "$installDir"
	fakeroot bash -c "tar -C \"$RFSDIR\" --exclude ./opt/oralux/voxin/share --exclude ./opt/oralux/voxin/rfs32 --exclude ./opt/oralux/voxin/include -Jcf $tarball ."		
	rm -rf "$releaseDir"
	mkdir -p "$releaseDir"

	getPreviousSpeechdModules
	
	# released tarballs:
	# voxin-$VOXIN_VERSION.tgz: voxin-$VOXIN_VERSION/voxin-$LANG-$VOXIN_VERSION.tgz
	# voxin-$LANG-$VOXIN_VERSION.tgz: voxin-$VOXIN_VERSION/voxin-$LANG-$VOXIN_VERSION/
	#
	# release dir:
	# $PKGDIR/voxin-$VOXIN_VERSION/
	#                          voxin-$VOXIN_VERSION/voxin-$LANG-$VOXIN_VERSION
	#
	# voxin-update
	# TODO nve: specific name + update installer...
	if [ "$withTTS" != nve ]; then
		installerDir="$releaseDir"/voxin-$VOXIN_VERSION/voxin-update-$VOXIN_VERSION
		installerTar="$releaseDir"/voxin-update-$VOXIN_VERSION.tgz
		mkdir -p "$installerDir" || return 1
		rsync -a "$INSTALLDIR"/ "$installerDir"
	
		fakeroot bash -c "tar -C \"$releaseDir\" -zcf \"$installerTar\" voxin-$VOXIN_VERSION" || return 1
		rm -rf "$releaseDir/voxin-$VOXIN_VERSION"
		echo "$installerTar"
	fi
	rm -rf "$releaseDir/voxin-$VOXIN_VERSION"

	if [ -f "$files" ]; then
		local all
		local lang
		local voxind
		local t
		# the common tarballs (*all*, *voxind*) are expected to be placed at the beginning of the list
		for t in $(cat "$files"); do
		    tarball=$(eval echo $t)
			[ -e "$tarball" ] || leave "Tarball not found: $tarball" 1 
		    case "$tarball" in
				*voxind-*txz) voxind=$tarball;;
				*voxin-nve-all*txz) all=$tarball;;
				*voxin-viavoice*txz)
					lang=$(getLangFromViavoiceTarball)
					if [ "$lang" = "all" ]; then
						all=$tarball
					else
						buildVoiceTarball $lang $tarball $all
					fi
					;;
				*) ;;
			esac
		done

		if [ "$withTTS" = nve ]; then
			buildVoiceTarball update-ve $voxind $all
		fi
		
		local sha512=$BASE/check/$VOXIN_VERSION/voxin-$VOXIN_VERSION.sha512
		if [ -e "$sha512" ]; then
			rm "$sha512"
		else
			mkdir -p ${sha512%/*}
		fi
		cd "$releaseDir"
		for i in *.tgz; do
			sha512sum "$i" >> "$sha512"
		done
		cd "$pwd"
		tar -C $(dirname "$releaseDir") -zcf "$releaseTarball" $(basename "$releaseDir")
	fi
}

# populate $INSTALLDIR
buildInstallerDir() {
	local pwd=$PWD

	rm -rf "$INSTALLDIR"
	mkdir -p "$INSTALLDIR"
	rsync --exclude .gitignore --exclude *~ --exclude *bak -a "$BASE"/src/installer/* "$INSTALLDIR"
	cd "$INSTALLDIR"

	emacs README.org --batch -f org-ascii-export-to-ascii --kill
	sed -e "s/%VERSION%/$VOXIN_VERSION/g" -e '/^[[:space:]]*[=~_]\+$/d' README.txt > README
	rm README.org README.txt README.txt~

	sed -i "s/%VERSION%/$VOXIN_VERSION/g" common/init.inc
	
	cd po
	for i in de es fr; do
		./install_po.sh $i
	done

	cd "$pwd"
}


# output buildDebPackageRes 
buildDebPackage() {
	[ "$#" -lt 8 ] && return 1
	local tarball=$1
	local installDir=$2
	local name=$3
	local version=$4
	local arch=$5
	local depends=$6
	local title=$7
	local description=$8
	local replaces=$9
	local pwd=$PWD

	unset buildDebPackageRes 
	which md5sum > /dev/null || leave "install md5sum" 1

	local pacdir="$TMPDIR"/buildDebPackage
	[ -e "$pacdir" ] && rm -rf "$pacdir"
	mkdir -p "$pacdir"/{DEBIAN,data}
	cd "$pacdir"/data

	tar -xf "$tarball"
	mv usr/share/doc/"$name"/LICENSE usr/share/doc/"$name"/copyright
	fakeroot bash -c "tar -Jcf ../data.tar.xz ."
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

	[ -n "$replaces" ] && echo -e "Conflicts: $replaces\nReplaces: $replaces" >> DEBIAN/control
	echo -e "Description: $title" >> DEBIAN/control
	local prefix=" "
	echo "$description" | fold -w 80 -s | sed "s/^/$prefix/" >> DEBIAN/control

	echo 2.0 > debian-binary
	fakeroot bash -c "tar -C DEBIAN -Jcf control.tar.xz ./control ./md5sums"

	[ ! -d "$installDir" ] && mkdir -p "$installDir"
	ar -r "$installDir"/${name}_${version}_${arch}.deb debian-binary control.tar.xz data.tar.xz 
	buildDebPackageRes="$installDir"/${name}_${version}_${arch}.deb
	cd "$pwd"
}

# output buildPacmanPackageRes
buildPacmanPackage() {
	[ "$#" -lt 9 ] && return 1
	local tarball=$1
	local installDir=$2
	local name=$3
	local version=$4
	local arch=$5
	local depends=$6
	local title=$7
	local description=$8
	local license=$9
	local replaces=${10}
	local date=$(date --utc +%s)
	local pwd=$PWD

	version=$version-1
	
	local pacdir="$TMPDIR"/buildPacmanPackage
	[ -e "$pacdir" ] && rm -rf "$pacdir"
	mkdir -p "$pacdir"
	cd "$pacdir"

	tar -xf "$tarball"
	mkdir -p usr/share/licenses/"$name"
	mv usr/share/doc/"$name"/LICENSE usr/share/licenses/"$name"/
	
	local size=$(du -scb . | head -n1 | cut -f1)

	cat <<EOF>.PKGINFO
pkgname = $name
pkgver = $version
pkgdesc = $title
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
tar -pcf "$installDir/$name-$version-$arch.pkg.tar" * .PKGINFO .MTREE 2> /dev/null
xz -z -9 "$installDir/$name-$version-$arch.pkg.tar"
EOF
	buildPacmanPackageRes="$installDir/$name-$version-$arch.pkg.tar.xz"
	cd "$pwd"		
}

# output buildGentooPackageRes
buildGentooPackage() {
	[ "$#" -lt 9 ] && return 1
	local tarball=$1
	local installDir=$2
	local name=$3
	local version=$4
	local arch=$5
	local depends=$6
	local title=$7
	local description=$8
	local license=$9
	local replaces=${10}
	local date=$(date --utc +%s)
	local pwd=$PWD
	
	local pacdir="$TMPDIR"/buildGentooPackage
	[ -e "$pacdir" ] && rm -rf "$pacdir"
	mkdir -p "$pacdir"
	cd "$pacdir"

	tar -xf "$tarball"
	mkdir -p usr/share/doc/"$name-$version"
	mv usr/share/doc/"$name"/{LICENSE,README} usr/share/doc/"$name-$version"/
	rm -rf usr/share/doc/"$name"
	local size=$(du -scb . | head -n1 | cut -f1)

	cat <<EOF>.PKGINFO
pkgname = $name
pkgver = $version
pkgdesc = $title
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
	buildGentooPackageRes="$installDir/$name-$version-$arch.pkg.tar.xz"
	cd "$pwd"		
}

# output buildRpmPackageRes
buildRpmPackage() {	
	[ "$#" -lt 9 ] && return 1
	local tarball=$1
	local installDir=$2
	local name=$3
	local version=$4
	local arch=$5
	local depends=$6
	local title=$7
	local description=$8
	local license=$9
	local replaces=${10}
	local date=$(date --utc +%s)
	local pwd=$PWD

	unset buildRpmPackageRes
	
	local pacdir="$TMPDIR"/buildRpmPackage
	if [ -e "$pacdir" ]; then
		find "$pacdir" -type d -exec chmod a+w {} \;
		rm -rf "$pacdir"
	fi
	mkdir -p "$pacdir/data"
	cd "$pacdir/data"
	tar -xf "$tarball"
	find * -type d -exec chmod a-w {} \;
	chmod 755 usr usr/include usr/share usr/share/doc usr/lib/speech-dispatcher-modules 2>/dev/null
	chmod 755 usr/share/speech-dispatcher usr/share/speech-dispatcher/conf usr/share/speech-dispatcher/conf/modules 2>/dev/null
	chmod 755 etc etc/speech-dispatcher etc/speech-dispatcher/modules 2>/dev/null
	[ "$arch" = "x86_64" ] && mv usr/lib usr/lib64
	[ ! -d "$installDir" ] && mkdir -p "$installDir"	
	tar -zcf "$pacdir/$name.tgz" *
	cd ..
	fakeroot alien --target="$arch" --version="$version" --description="$title. $description" --to-rpm "$name.tgz"
	local rpm=$(ls *rpm)
	cp "$rpm" "$installDir"
	buildRpmPackageRes="$installDir/$rpm"
	cd "$pwd"
}

# output buildSlackwarePackageRes
buildSlackwarePackage() {	
	[ "$#" -lt 9 ] && return 1
	local tarball=$1
	local installDir=$2
	local name=$3
	local version=$4
	local arch=$5
	local depends=$6
	local title=$7
	local description=$8
	local license=$9
	local date=$(date --utc +%s)
	local pwd=$PWD

	unset buildSlackwarePackageRes
	
	local pacdir="$TMPDIR"/buildSlackwarePackage
	if [ -e "$pacdir" ]; then
		find "$pacdir" -type d -exec chmod a+w {} \;
		rm -rf "$pacdir"
	fi
	mkdir -p "$pacdir/data"
	cd "$pacdir/data"
	tar -xf "$tarball"
	mkdir -p usr/doc/"$name"-$VOXIN_VERSION
	mv usr/share/doc/"$name"/LICENSE usr/share/doc/"$name"/README usr/doc/"$name"-$VOXIN_VERSION
	rm -rf usr/share/doc/"$name"
	
	# inspired by xz2txz.sh
	# https://forum.porteus.org/viewtopic.php?f=39&t=4577#p36115
	local prefix="$name: "
	local descFile=install/slack-desc	
	[ "$arch" = "x86_64" ] && [ -d usr/lib ] && mv usr/lib usr/lib64
	find . -type d -exec chmod 755 {} \;
	[ ! -d install ] && mkdir install
	echo "$prefix$name ($title)" > $descFile
	echo "$prefix" >> $descFile
	echo "$description" | fold -w 69 -s | sed "s/^/$prefix/" >> $descFile
	local n=$(wc -l "$descFile" | cut -f1 -d" ")
	local i
	for ((i=0; i<$((11-n)); i++)); do echo "$prefix"; done >> $descFile
	
	local script=install/doinst.sh
	local DEST
	local DIR
	local SRC
	touch $script
	for i in $(find . -type l); do
		DEST=${i##*/}
		DIR=${i%/*}
		SRC=$(readlink "$i")
		if [ -z "$DEST" ] || [ -z "$DIR" ] || [ -z "$SRC" ]; then return; fi
		echo "( cd $DIR ; rm -rf $DEST )" >> "$script"
		echo "( cd $DIR ; ln -sf $SRC $DEST )" >> "$script"
		unlink $i
	done

	# installation
	[ ! -d "$installDir" ] && mkdir -p "$installDir"		
	fakeroot << EOF
tar -pcf "$installDir/$name-$version-$arch-1" * 2> /dev/null
xz -S .txz -z -9 "$installDir/$name-$version-$arch-1"
EOF
	buildSlackwarePackageRes="$installDir/$name-$version-$arch-1.txz"
	cd "$pwd"
}

# getMajMinVersion
# Example:
# input=1.2.3-4.fc29
# output=1.2
getMajMinVersion() {
	[ $# != 1 ] && return
	echo "$1" | cut -f1,2 -d.
}

downloadVoxinUpdateFromX86VM() {
	local TARBALL="voxin-installer/build/packages/voxin-$VOXIN_VERSION/voxin-update-$VOXIN_VERSION.tgz"
	mkdir -p "$DWLX86"
	scp "$VMX86:$TARBALL" "$DWLX86"	
}

uploadToX86VM() {
	local pwd=$PWD
	cd $BASE/..

	local SRC="voxin-installer voxin-installer/build/packages/buildroot_2017.02.9.txz voxin-installer/build/.gitignore"
	rsync --exclude build --exclude tmp -aRvz $SRC $VMX86:

	cd $HOME
	local VVDIR=VOXIN/voxin-viavoice/build/packages
	if [ -e "$HOME/$VVDIR" ]; then
		rsync -aRvz $VVDIR/voxin-viavoice-* $VMX86:
	fi

	VVDIR=VOXIN/voxin-nve/build/packages
	if [ -e "$HOME/$VVDIR" ]; then
		rsync -aRvz $VVDIR/voxin-nve-* $VMX86:
	fi

	ssh $VMX86 'cd voxin-installer && ./build.sh -t src/list.vv'
	ssh $VMX86 'cd voxin-installer && ./build.sh -t src/list.ve'
	cd "$pwd"
}

getx86Arch() {
	local TARBALL="$DWLX86/voxin-update-$VOXIN_VERSION.tgz"
	if [ -e "$TARBALL" ]; then
		tar -C "$INSTALLDIR" --exclude rfs_$VOXIN_VERSION.all.txz --strip-components=2 -xf "$TARBALL" voxin-$VOXIN_VERSION/voxin-update-$VOXIN_VERSION/packages		
	fi
}
