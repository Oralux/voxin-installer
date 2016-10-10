# libvoxin/voxind program version
MAJ=1
MIN=1
REV=2

# packages version
PKG=1

# github url + sha256sum
ARCHIVE=$MAJ.$MIN.$REV.tar.gz
URL=https://github.com/Oralux/libvoxin/archive/$ARCHIVE
SHA256=6ec242b25c940701ce57ff20bd81ebf76304aeee76373c824d8a117f43aa87e7

# gnupg key used to sign packages
SIGNING_KEY=AED093E5


download()
{
    if [ ! -e "$ARCHIVE" ]; then
	wget $URL
    fi
    a=$(sha256sum "$ARCHIVE" | cut -f1 -d" ")
    if [ "$a" != "$SHA256" ]; then
	echo "$ARCHIVE: sha256 mismatch !"
	exit 1
    fi
}

extract()
{
    tar -zxf "$ARCHIVE"
}
