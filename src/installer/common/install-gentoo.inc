gentooTarExclude="--exclude .MTREE --exclude .PKGINFO"

getVoxinPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/gentoo -name "voxin*$ARCH.pkg.tar.xz"
}

getPackageVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	find /var/db/pkg -name "${package}-[0-9]*" -type d | sed -r "s/.*-([0-9]+.*)/\1/"
}

isPackageInstalled() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local package=$1
	local a=$(find /var/db/pkg -name "${package}-[0-9]*" -type d)
	[ -n "$a" ]
}

# getPackageFileVersion
# Example:
# input: name1-name2-1.2.3-1-x86_64.pkg.tar.xz
# output: 1.2
getPackageFileVersion() {
	unset output
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	output=$(echo "$package" | sed -E -e "s/-$ARCH.pkg.tar.xz//" -e "s/.*-([[:digit:]]+.[[:digit:]]+).*/\1/")
}

installLocalPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$(readlink -e "$1")
	[ -z "$package" ] && return 1
	# --skip-old-file: dont' replace existing lib symlink (/usr/lib -> lib64) by a directory
	tar -C / $gentooTarExclude --skip-old-file -xf "$package" &>>"$LOG"
}

installPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	emerge "$package" &>> "$LOG"
}

uninstallPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	local tarball
	local i

	[ "$package" != voxin ] && return 1
	tarball=$(find packages/gentoo -name "voxin*$ARCH.pkg.tar.xz" 2>/dev/null)
	[ -z "$tarball" ] && return 1
	tarball=$(readlink -e "$tarball")
	[ -z "$tarball" ] && return 1

	pwd=$PWD
	cd /
	tar $gentooTarExclude -tf "$tarball" | while read i; do
	    [ ! -h "$i" ] && continue
	    target=$(readlink -m $i)
	    case "$target" in
			/opt/oralux/*) unlink "$i";;
	    esac
	done	
	cd $pwd
}

