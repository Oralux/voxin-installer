# The BASE variable should be set before including this file
#
VOXIN_VER=2.0

#-- voxin-rfs32
# Buildroot
BR_VER=2017.02.9
BR_URL=https://buildroot.org/downloads/buildroot-${BR_VER}.tar.gz
# BR_PKG defines below

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
#####

PKGDIR=$BASE/build/packages
TMPDIR=$BASE/build/tmp
RFS32=/opt/oralux/voxin/rfs32
BR_PKG="$PKGDIR/buildroot_${BR_VER}.txz"
RFS_PKG="$PKGDIR/rfs_${VOXIN_VER}.txz"
RFS_DIR=$TMPDIR/rfs
#RFS32_PKG="$PKGDIR/${NAME}.txz"

# gnupg key used to sign packages
SIGNING_KEY=AED093E5

debianIsPackageInstalled() {
	local deb=$1
	grep -A1 "Package: $deb$" /var/lib/dpkg/status | grep "Status: install ok installed" &> /dev/null
}

debianDoesPackageExist() {
	local deb=$1
	grep "Package: $deb$" /var/lib/apt/lists/*_Packages &> /dev/null
}

leave() {
		echo -e "$1" && exit $2
}
