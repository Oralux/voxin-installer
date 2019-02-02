getVoxinPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/archlinux -name "libvoxin*$ARCH.pkg.tar.xz"
}

getPackageVersion() {
	[ $# != 1 ] && return
	local deb=$1
	pacman -Q "$deb" 2>/dev/null | awk '{print $2}'
}

isPackageInstalled() {
	[ $# != 1 ] && return 1
	local deb=$1
	pacman -Q "$deb" &>/dev/null
}

# getPackageFileVersion
# Example:
# input: name1-name2-1.2.3-1-x86_64.pkg.tar.xz
# output: 1.2
getPackageFileVersion() {
	[ $# != 1 ] && return
	local package=$1
	echo "$package" | sed -E -e "s/-$ARCH.pkg.tar.xz//" -e "s/.*-([[:digit:]]+.[[:digit:]]+).*/\1/" 
}

# getListSpeechDispatcherVoxin
# output: 2 vars
# getListSpeechDispatcherVoxin1: list of sd_ibmtts packages
# getListSpeechDispatcherVoxin2: list of ibmtts.conf packages
getListSpeechDispatcherVoxin() {
	getListSpeechDispatcherVoxin1=$(find packages/archlinux -name "speech-dispatcher-voxin*$ARCH.pkg.tar.xz" ! -name "speech-dispatcher-voxin-conf*")
	getListSpeechDispatcherVoxin2=$(find packages/archlinux -name "speech-dispatcher-voxin-conf*$ARCH.pkg.tar.xz")
}

installLocalPackage() {
	[ $# != 1 ] && return
	local package=$1
	pacman -U --noconfirm "$package" &>>"$LOG"
}

installPackage() {
	[ $# != 1 ] && return
	local package=$1
    pacman -S --noconfirm "$package" &>>"$LOG"
}

uninstallPackage() {
	[ $# != 1 ] && return
	local package=$1
	pacman --noconfirm -R "$package" &>>"$LOG"
}

