# THE BASE variable should be set before including this file
#
VOXIN_VERSION=3.3rc6

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
LIBVOXIN_VERSION=1.6.0
LIBVOXIN_SHA512=157ef39ae9c0b88e48742dc9b9ee8220dcc81b4c24a8971a8f06659bf7c51e6566cf1a1fc7862c2eb19b1717fc58e849ecef2c4f343083931a5b595ad54f996b

# versions of the speech-dispatcher voxin module(s) which must be
# built by this script.
# 
SPEECHD_VOXIN_ALL_VERSIONS=
#SPEECHD_VOXIN_ALL_VERSIONS=0.10.1
#SPEECHD_VOXIN_ALL_VERSIONS="0.10.1 0.9.1 0.9.0"

SPEECHD_VOXIN_ARCHIVE_URL=https://github.com/Oralux/speechd/archive
SPEECHD_VOXIN_GIT_URL=https://github.com/Oralux/speechd.git
SPEECHD_VOXIN_TAG=voxin3
SPEECHD_VOXIN_TAG_0_10_1=voxin3_backport_upstream
SPEECHD_VOXIN_TAG_0_9_1=voxin3rc5
SPEECHD_VOXIN_TAG_0_9_0=voxin3rc5
SPEECHD_SD_MODULE_COPYRIGHT_NB_LINES=24

# pre-built binaries of the speech-dispatcher voxin module(s) to download.
# Built from sources identified by the sources.ini file inside the tarball.
SPEECHD_VOXIN_BIN_URL=http://oralux.org/soft/speech-dispatcher/bin
SPEECHD_VOXIN_BIN_ARCHIVES=" \
sd_voxin_0.7.1_0.8.8.x86_64.txz \
sd_voxin_0.9.0-voxin3rc5.aarch64.txz \
sd_voxin_0.9.0-voxin3rc5.armv7l.txz \
sd_voxin_0.9.0-voxin3rc5.x86_64.txz \
sd_voxin_0.9.1-voxin3rc5.aarch64.txz \
sd_voxin_0.9.1-voxin3rc5.armv7l.txz \
sd_voxin_0.9.1-voxin3rc5.x86_64.txz \
sd_voxin_0.10.1-voxin3_backport_upstream.aarch64.txz \
sd_voxin_0.10.1-voxin3_backport_upstream.armv7l.txz \
sd_voxin_0.10.1-voxin3_backport_upstream.x86_64.txz \
"

#"sd_voxin_0.7.1_0.8.8.x86_64.txz sd_voxin_0.9.1-voxin3.x86_64.txz"
SPEECHD_VOXIN_BIN_SHA512=$BASE/check/$VOXIN_VERSION/sd_voxin.sha512

# VMX86_64: VM where voxin-installer could be built for x86_64 architecture
# Debian 8 to build binaries compatible with older distros. 
# Ubuntu 18.04 for sd_voxin 0.9.0 and 0.9.1
# 
#VMX86_64=user1@192.168.122.150 # Ubuntu 18.04 (sd_voxin 0.9.0 and 0.9.10)
VMX86_64=user1@192.168.122.118 # Debian 8 (libvoxin, sd 0.7, 0.8)
VMVOXDIR=/home/user1/VOXIN
VMLIBVOXDIR=$VMVOXDIR/LIBVOXIN

# VMX86: VM where voxin-installer is built for x86 architecture
# This VM is currently used to build on a 64 bits machine the
# speech-dispatcher module for x86
# 
VMX86=user1@192.168.122.174

# Dependencies
# # libstdc++-4.9-dev:i386 (hard coded in function below)
# DEP_PACKAGES="alien libarchive-tools build-essential fakeroot rpm rsync build-essential binutils-multiarch libc6-dev-i386 gcc-multilib g++-multilib patchelf"
# DEP_PACKAGES="alien libarchive-tools build-essential fakeroot rpm rsync"
# Debian8: speech-dispatcher dependencies
# obtained with apt-get build-dep speech-dispatcher

DEP_PACKAGES="alien build-essential fakeroot rpm rsync"
[ "$(uname -m)" == x86_64 ] && DEP_PACKAGES="$DEP_PACKAGES gcc-multilib"

# bsdtar (for Arch/Gentoo package)
for i in libarchive-tools bsdtar; do
    { apt-cache search "$i"| grep -o "^$i "; } && DEP_PACKAGES="$DEP_PACKAGES $i"
done

DEP_PACKAGES_X86="alien libarchive-tools rpm build-essential flite1-dev intltool libao-dev libasound2-dev libaudio-dev libdotconf-dev libespeak-dev libflac-dev libglib2.0-dev libogg-dev libpulse-dev libsndfile1-dev libvorbis-dev libxau-dev pkg-config tex-common x11proto-core-dev xorg-sgml-doctools zlib1g-dev dh-autoreconf libltdl-dev libtool"

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
BUILD_VOXIN_PKG=$TMPDIR/buildVoxinPackage
BUILD_VOXIN_SPEECHD_PKG=$TMPDIR/buildVoxinSpeechdPackage
BUILD_VOXIN_SPEECHD_CONF_PKG=$TMPDIR/buildVoxinSpeechdConfPackage
DWLX86=$DWLDIR/x86

DOC=$VOXINDIR/usr/share/doc

# gnupg key used to sign packages
SIGNING_KEY=AED093E5

getArch() {
    ARCH=$(uname -m)
    case "$ARCH" in
        x86|i386|i586|i686) ARCH=i686;;
#        armv7l) ;;
#        aarch64) ;;
    esac
}

# set the variables describing the speech-dispatcher voxin archive
# (filename, sha sum)
setSpeechdVoxinVar() {
    local version=$1

    unset SPEECHD_VOXIN_ARCHIVE SPEECHD_VOXIN_SHA512
    case $version in
	0.10.1)	    
#	    SPEECHD_VOXIN_ARCHIVE=$version-$SPEECHD_VOXIN_TAG.tar.gz
	    SPEECHD_VOXIN_ARCHIVE=$version-$SPEECHD_VOXIN_TAG_0_10_1.tar.gz
	    SPEECHD_VOXIN_SHA512=9a5f849fa92042913ee0401912fe4c0e3836ac2428e016aebd3e587e5c6f93ff14b53337a525fa51f1d866d35f44ef09f748741b4863522a70b9deaf2ef9eaf0
	    ;;
	0.9.1)
	    SPEECHD_VOXIN_ARCHIVE=$version-$SPEECHD_VOXIN_TAG_0_9_1.tar.gz
	    SPEECHD_VOXIN_SHA512=153475db5bc8a8cd838795f18e25e35b6199dc05f7d26cfae79ce719cce181ccee3532446d160f1982962a266c2e13c5aed9bbd3e3613c248171d8ec1fce1aff
	    ;;
	0.9.0)	    
	    SPEECHD_VOXIN_ARCHIVE=$version-$SPEECHD_VOXIN_TAG_0_9_0.tar.gz
	    SPEECHD_VOXIN_SHA512=b13461421014f328a7f5ac2d3e3acd61e794ceae8cf9f26c577a3107fa4b44ef196e2e5b87407a0153bef4ada9a9e0d9e87584734ff7f37e09d59ca8466cd675
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
	for i in "$TMPDIR" "$BUILD_VOXIN_PKG" "$BUILD_VOXIN_SPEECHD_PKG" "$BUILD_VOXIN_SPEECHD_CONF_PKG" "$PKGDIR"/voxin* "$PKGDIR"/rfs* "$RFS32"; do
		rm -rf "$i"
	done
	mkdir -p "$PKGDIR" "$DWLDIR" "$BUILD_VOXIN_PKG"/{rfs,build} "$BUILD_VOXIN_SPEECHD_PKG"/{rfs,build} "$BUILD_VOXIN_SPEECHD_CONF_PKG"/{rfs,build}
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

getSpeechdVoxinBinTarballs() {
	local dwldir="$DWLDIR/previous_speechd_voxin"
#	local archivep="$dwldir/$PREV_SPEECHD_VOXIN_ARCHIVE"
	local installDir=$INSTALLDIR/packages/all
	local pwd=$PWD

	[ ! -d "$dwldir" ] && mkdir -p "$dwldir"

	cd "$dwldir"
	for file in $SPEECHD_VOXIN_BIN_ARCHIVES; do
	    if [ ! -e "$file" ]; then
		wget "$SPEECHD_VOXIN_BIN_URL/$file"
		if [ ! -e "$file" ]; then
			leave "$file not found" 1
		fi
	    fi
	done
	sha512sum -c "$SPEECHD_VOXIN_BIN_SHA512" || leave "speechd_voxin: sha512 mismatch!" 1

	for file in $SPEECHD_VOXIN_BIN_ARCHIVES; do
	    cp -a "$file" "$installDir"
	done

	cd "$pwd"	
}

getDebArch() {
	local arch=$1
	case "$arch" in
		x86_64) arch=amd64;;
		arm*) arch=armhf;;
		aarch64) arch=arm64;;
		*) arch=i386;;
	esac
	echo $arch
}

getPacmanArch() {
	local arch=$1
	case "$arch" in
		armv7*) arch=armv7h;; # armv7 hardfloat
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
	tar -C "$BUILD_VOXIN_PKG/rfs" -xf "$tarball" || leave "Can't uncompress libvoxin-pkg tarball ($tarball)" 1

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
	
	[ ! -d "$installDir" ] && mkdir -p "$installDir"

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

	./build.sh
	CFLAGS="-I$libvoxinDir/../include/voxin" LDFLAGS="-L$libvoxinDir" ./configure --prefix="/usr"
	make || return 1
	
	# install
	local rfsdir="$TMPDIR"/sd_voxin
	local modulesDir="$rfsdir"/opt/oralux/voxin/lib/speech-dispatcher-modules/$version
	local confDir="$rfsdir"/opt/oralux/voxin/share/speech-dispatcher/$version
	local docDir="$rfsdir"/opt/oralux/voxin/share/doc/speech-dispatcher-voxin/$version
	local module=src/modules/sd_voxin
#	local sd_tag=$version-$SPEECHD_VOXIN_TAG
	local sd_tag=${SPEECHD_VOXIN_ARCHIVE%.tar.gz}
	local tarball="$installDir"/sd_voxin_$sd_tag.$arch.txz

	rm -rf $rfsdir
	mkdir -p "$modulesDir" "$confDir" "$docDir"
	[ -e src/modules/.libs/sd_voxin ] && module=src/modules/.libs/sd_voxin
	cp "$module" "$modulesDir" || return 1
	strip -s "$modulesDir/sd_voxin"
	cp ./config/modules/voxin.conf "$confDir" || return 1
	local file=src/modules/voxin.c
	[ ! -e "$file" ] && file=src/modules/ibmtts.c
	[ ! -e "$file" ] && return 1
	head -n $SPEECHD_SD_MODULE_COPYRIGHT_NB_LINES $file > "$docDir"/copyright
	cat<<EOF>"$docDir"/sources.ini
[archive]
url=$SPEECHD_VOXIN_ARCHIVE_URL/$SPEECHD_VOXIN_ARCHIVE
sha512=$SPEECHD_VOXIN_SHA512

[git]
url=$SPEECHD_VOXIN_GIT_URL
tag=$sd_tag
EOF

	fakeroot bash -c "\
tar -C \"$rfsdir\" \
	   -Jcf \"$tarball\" opt/oralux/voxin/share/doc/speech-dispatcher-voxin/$version opt/oralux/voxin/lib/speech-dispatcher-modules/$version opt/oralux/voxin/share/speech-dispatcher/$version" leave "can't create tarball into ${tarball%/*}" 1

	rm -rf $modulesDir $confDir $docDir
	cd "$pwd"
	return 0
}

# build the sd_voxin launcher
buildVoxinModuleLauncher() {
	local pwd="$PWD"
	local modulesDir="$VOXINDIR"/lib/speech-dispatcher-modules
	cd "$SRCDIR"/speechd-voxin
	make clean
	make
	DESTDIR="$modulesDir" make install

	# Speech Dispatcher (>= 0.10.2) can build a Voxin module.
	# For example, this module is already included in Arch Linux
	# (December 2020, speech-dispatcher package) or in Debian sid
	# (2020, speech-dispatcher-voxin package).
	#
	# If a GNU/Linux distribution does not already provide this
	# Voxin module, the voxin-speechd and voxin-speechd-conf
	# packages provided in the tarball will be installed.
	#
	# 1. voxin-speechd
	# voxin-speechd supplies a symbolic link to a module selector:
	# /usr/lib/speech-dispatcher-modules/sd_voxin
	# which points to
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
	# 2. voxin-speechd-conf
	# voxin_speechd_conf supplies a symbolic link from:
	# /etc/speech-dispatcher/modules/voxin.conf
	# to 
	# /opt/oralux/voxin/share/speech-dispatcher/conf/voxin.conf
	#
	# which itself points on the configuration matching the current version of speech-ditacher
	# The default configuration files are stored under
	# /opt/oralux/voxin/share/speech-dispatcher/conf/
	#   0.9.1/voxin.conf
	#   0.9.0/voxin.conf
	#

	# create symlinks
	cd "$BUILD_VOXIN_SPEECHD_PKG/rfs"
	mkdir -p usr/lib/speech-dispatcher-modules
	ln -s /opt/oralux/voxin/lib/speech-dispatcher-modules/sd_voxin usr/lib/speech-dispatcher-modules/sd_voxin
#	mkdir -p usr/{lib/speech-dispatcher-modules,libexec/speech-dispatcher-modules}
#	ln -s /opt/oralux/voxin/lib/speech-dispatcher-modules/sd_voxin usr/libexec/speech-dispatcher-modules/sd_voxin

	cd "$BUILD_VOXIN_SPEECHD_CONF_PKG/rfs"
	mkdir -p etc/speech-dispatcher/modules
	ln -s /opt/oralux/voxin/share/speech-dispatcher/voxin.conf etc/speech-dispatcher/modules/voxin.conf

	# omitted: to avoid to create of a third package...
	# cd "$BUILD_VOXIN_SPEECHD_CONF2_PKG/rfs"
	# mkdir -p usr/share/speech-dispatcher/conf/modules
	# ln -s /opt/oralux/voxin/share/speech-dispatcher/voxin.conf usr/share/speech-dispatcher/conf/modules/voxin.conf
	
	cd "$pwd"
}

# install the README and LICENSE files
# create the symlinks for the voxin package
getVoxinDoc() {
	local pwd=$PWD

	cp $INSTALLDIR/README $INSTALLDIR/LICENSE "$VOXINDIR"

	cd "$BUILD_VOXIN_PKG/rfs"
	mkdir -p usr/share/doc/voxin
	ln -s /opt/oralux/voxin/LICENSE usr/share/doc/voxin/
	ln -s /opt/oralux/voxin/README usr/share/doc/voxin/
	
	cd "$BUILD_VOXIN_SPEECHD_PKG/rfs"
	mkdir -p usr/share/doc/voxin-speechd
	ln -s /opt/oralux/voxin/LICENSE usr/share/doc/voxin-speechd/
	ln -s /opt/oralux/voxin/README usr/share/doc/voxin-speechd/
	
	cd "$BUILD_VOXIN_SPEECHD_CONF_PKG/rfs"
	mkdir -p usr/share/doc/voxin-speechd-conf
	ln -s /opt/oralux/voxin/LICENSE usr/share/doc/voxin-speechd-conf/
	ln -s /opt/oralux/voxin/README usr/share/doc/voxin-speechd-conf/
	
	cd "$pwd"
	return 0
}

buildVoxinPackage() {
	[ $# != 1 ] && return 1
	local arch=$1
	local tarball="$BUILD_VOXIN_PKG"/build/voxin_$VOXIN_VERSION.all.txz
	local rfsdir=$BUILD_VOXIN_PKG/rfs
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
	replaces="libvoxin1, libvoxin1-dev, voxind"
	installDir=$INSTALLDIR/packages/deb
	buildDebPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$debArch" "$depends" "$title" "$description" "$replaces"

	# gentoo
	local depends=glibc
	replaces="libvoxin, voxind"
	installDir=$INSTALLDIR/packages/gentoo
	buildGentooPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# pacman
	local depends=glibc
	replaces="libvoxin, voxind"
	installDir=$INSTALLDIR/packages/archlinux
	buildPacmanPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# rpm
	replaces="libvoxin, libvoxin-debuginfo, libvoxin-devel, voxind, voxind-debuginfo"
	installDir=$INSTALLDIR/packages/rpm
	buildRpmPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces"
	
	# slackware
	installDir=$INSTALLDIR/packages/slackware
	buildSlackwarePackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+"
}

buildVoxinSpeechdPackage() {
	[ $# != 1 ] && return 1
	local arch=$1
	local tarball="$BUILD_VOXIN_SPEECHD_PKG"/build/voxin_speechd_$VOXIN_VERSION.all.txz
	local rfsdir=$BUILD_VOXIN_SPEECHD_PKG/rfs

	fakeroot bash -c "\
tar -C \"$rfsdir\" \
	   -Jcf \"$tarball\" ." leave "can't create tarball into $rfsdir" 1

	# package voxin_speechd for various package managers
	local title="voxin-speechd pseudo package"
	local description="Provides symbolic links to the Voxin module for Speech Dispatcher. This package is only needed if the distribution does not already supply sd_voxin."

	local name=voxin-speechd
	local depends=libc6
	local replaces

	# debian
	local debArch=$(getDebArch "$arch")
	replaces="speech-dispatcher-voxin"
	installDir=$INSTALLDIR/packages/deb
	buildDebPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$debArch" "$depends" "$title" "$description" "$replaces"

	# gentoo
	local depends=glibc
	replaces="speech-dispatcher-voxin"
	installDir=$INSTALLDIR/packages/gentoo
	buildGentooPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# pacman
	# depends: use space as separator
	local depends=glibc
	replaces="speech-dispatcher-voxin"
	installDir=$INSTALLDIR/packages/archlinux
	buildPacmanPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# rpm
	replaces="speech-dispatcher-voxin"
	installDir=$INSTALLDIR/packages/rpm
	buildRpmPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces"
	
	# slackware
	installDir=$INSTALLDIR/packages/slackware
	buildSlackwarePackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+"
}

buildVoxinSpeechdConfPackage() {
	[ $# != 1 ] && return 1
	local arch=$1
	local tarball="$BUILD_VOXIN_SPEECHD_CONF_PKG"/build/voxin_speechd_conf_$VOXIN_VERSION.all.txz
	local rfsdir=$BUILD_VOXIN_SPEECHD_CONF_PKG/rfs
	fakeroot bash -c "\
tar -C \"$rfsdir\" \
	   -Jcf \"$tarball\" ." leave "can't create tarball into $rfsdir" 1

	# package voxin_speechd_conf for various package managers
	local title="voxin-speechd-conf pseudo package"
	local description="Provides symbolic links to the Voxin module for Speech Dispatcher. This package is only needed if the distribution does not already supply voxin.conf."

	local name=voxin-speechd-conf
	local depends=libc6
	local replaces=

	# debian
	local debArch=$(getDebArch "$arch")
	replaces="speech-dispatcher-voxin"
	installDir=$INSTALLDIR/packages/deb
	buildDebPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$debArch" "$depends" "$title" "$description" "$replaces"

	# gentoo
	local depends=glibc
	replaces="speech-dispatcher-voxin"
	installDir=$INSTALLDIR/packages/gentoo
	buildGentooPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# pacman
	# depends: use space as separator
	local depends=glibc
	replaces="speech-dispatcher-voxin"
	installDir=$INSTALLDIR/packages/archlinux
	buildPacmanPackage "$tarball" "$installDir" "$name" "$VOXIN_VERSION" "$arch" "$depends" "$title" "$description" "LGPL-2.1+" "$replaces" 
	
	# rpm
	replaces="speech-dispatcher-voxin"
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
	local arch=$4
	local releaseDir=$PKGDIR/voxin-$VOXIN_VERSION
	local installDir=$INSTALLDIR/packages/all
	local installerDir="$releaseDir/voxin-$VOXIN_VERSION/voxin-$voice-$VOXIN_VERSION"
	local installerTar="$releaseDir/voxin-$voice-$VOXIN_VERSION.$arch.tgz"

	mkdir -p "$installerDir" || return 1
	rsync -a "$INSTALLDIR"/ "$installerDir"
	cp -a "$tarball" "$all" "$installerDir"/packages/all
	fakeroot bash -c "tar -C \"$releaseDir\" --exclude \"*~\" -zcf \"$installerTar\" voxin-$VOXIN_VERSION" || return 1
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
	local releaseTarball=$PKGDIR/${withTTS}_voxin-$VOXIN_VERSION.$arch.tgz
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

	getSpeechdVoxinBinTarballs
	
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
				*voxind-*.$arch.txz) voxind=$tarball;;
				*voxin-nve-all*txz) all=$tarball;;
				*voxin-viavoice*txz)
					lang=$(getLangFromViavoiceTarball)
					if [ "$lang" = "all" ]; then
						all=$tarball
					else
						buildVoiceTarball $lang $tarball $all $arch
					fi
					;;
				*) ;;
			esac
		done

		local sha512=$BASE/check/$VOXIN_VERSION/voxin-$VOXIN_VERSION.sha512.vv
		if [ "$withTTS" = nve ]; then		    
			buildVoiceTarball update-ve $voxind $all $arch
			sha512=${sha512%.vv}.ve
		fi
		
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
# depends: use space as separator
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

	arch=$(getPacmanArch "$arch")
	
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
EOF

	for i in $depends; do echo "depend = $i" >> .PKGINFO; done

	
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

	# according to multilib.eclass
	case "$arch" in
	    *64*) [ -e usr/lib ] && mv usr/lib usr/lib64;;
	esac
	
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
	# acccording to Fedora Rawhide (jan 2021)
	case "$arch" in
	    *64) [ -e usr/lib ] && mv usr/lib usr/lib64;;
	    arm*) arch=armv7hl;;
	esac
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
