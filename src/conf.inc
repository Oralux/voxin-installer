#-- voxin-rfs32
BR_OUTPUT=/opt/BUILDROOT/buildroot-2017.02.9/output
BR_SRC=$BR_OUTPUT/target
BR_GLIBC_LICENSE=$BR_OUTPUT/build/glibc-2.23/LICENSES 

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
RFS32=/opt/voxin/rfs32

# gnupg key used to sign packages
SIGNING_KEY=AED093E5

getOverride() {
	local name=$1
	local dst=$2
	local label=$3
 	mkdir -p $dst/usr/share/lintian/overrides

#	echo "\
## The $name package adds files to a global 32 bits rootfilesystem used
#" > $dst/usr/share/lintian/overrides/$name
	rm -f $dst/usr/share/lintian/overrides/$name
 	for label in $label; do
 		echo "$name binary: $label" >> $dst/usr/share/lintian/overrides/$name
 	done
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

