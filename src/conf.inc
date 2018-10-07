#-- voxin-rfs32
BR_OUTPUT=/opt/BUILDROOT/buildroot-2017.02.9/output
BR_SRC=$BR_OUTPUT/target
BR_GLIBC_LICENSE=$BR_OUTPUT/build/glibc-2.23/LICENSES 

#-- voxin-libstdc++
LIBSTDCPP_ORIG_NAME=libstdc++2.10-glibc2.2
LIBSTDCPP_URL=http://archive.debian.org/debian/pool/main/g/gcc-2.95/${LIBSTDCPP_ORIG_NAME}_2.95.4-27_i386.deb

#-- libvoxin/voxind
LIBVOXIN_URL=https://github.com/Oralux/libvoxin/archive

# download sources from master branch (development)
LIBVOXIN_VERSION=
# uncomment the two following lines to download sources from the 1.3.2 tag instead
#LIBVOXIN_VERSION=1.3.2
#LIBVOXIN_SHA256=20c8e468258f0a314cb5524bec327b9ea591d32502afe2f80d7b43ca3bd0c1d5

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

