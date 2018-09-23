# libvoxin/voxind program version
MAJ=1
MIN=3
REV=0
# package version
PKG=1

# unset DEV to download tagged sources
# if DEV is set, the downloaded sources are from master branch
# (development branch) or otherwise the sources corresponding to
# tag=$MAJ.$MIN.$REV
#DEV=
DEV=1

URL=https://github.com/Oralux/libvoxin/archive
ARCHIVE=$MAJ.$MIN.$REV.tar.gz
SHA256=ea4ba781ce5b5098721558a781a5409d6d3611c1b9be50ce4a1b72a7743070f3

#####
VERSION=$MAJ.$MIN.$REV-$PKG
PKGDIR=$BASE/build/packages
TMPDIR=$BASE/build/tmp
RFS32=/opt/voxin/rfs32

# gnupg key used to sign packages
SIGNING_KEY=AED093E5

download()
{
	if [ -n "$DEV" ]; then
		ARCHIVE=master.zip
		unset SHA256
	fi
    if [ ! -e "$ARCHIVE" ]; then
	   	wget $URL/$ARCHIVE
    fi
	if [ -n "$SHA256" ]; then
		a=$(sha256sum "$ARCHIVE" | cut -f1 -d" ")
		if [ "$a" != "$SHA256" ]; then
			echo "$ARCHIVE: sha256 mismatch !"
			exit 1
		fi
	fi
}

extract()
{
	if [ -n "$DEV" ]; then
		[ ! -d libvoxin-master ] && unzip "$ARCHIVE" || true
	else
		[ ! -d libvoxin-$MAJ.$MIN.$REV ] && tar -zxf "$ARCHIVE" || true
	fi
}
