getVoxinPackage() {
	[ -z "$DEBIAN_ARCH" ] && return 1
	find packages/deb -name "libvoxin*_$DEBIAN_ARCH.deb"
}

getPackageVersion() {
	[ $# != 1 ] && return
	local deb=$1
	dpkg-query --showformat='${Version}' --show "$deb"
}

isPackageInstalled() {
	[ $# != 1 ] && return 1
	local deb=$1
	grep -A1 "Package: $deb$" /var/lib/dpkg/status | grep "Status: install ok installed" &> /dev/null
}

# getPackageFileVersion
# Example:
# input: name_1.2.3-5voxin1_amd64.deb
# output: 1.2
getPackageFileVersion() {
	[ $# != 1 ] && return
	local package=$1
	echo "$package" | sed 's+.*_\(.*\)-.*+\1+'
}

# getListSpeechDispatcherVoxin
# output: 2 vars
# getListSpeechDispatcherVoxin1: list of sd_ibmtts packages
# getListSpeechDispatcherVoxin2: list of ibmtts.conf packages
getListSpeechDispatcherVoxin() {
	getListSpeechDispatcherVoxin1=$(find packages/deb -name "speech-dispatcher-voxin_*_$DEBIAN_ARCH.deb")
	getListSpeechDispatcherVoxin2=$(find packages/deb -name "speech-dispatcher-voxin-conf_*_$DEBIAN_ARCH.deb")
}

installLocalPackage() {
	[ $# != 1 ] && return
	local package=$1
	dpkg -i "$package" &>> "$LOG"
}

installPackage() {
	[ $# != 1 ] && return
	local package=$1
    apt-get -q install --yes "$package" &>> "$LOG"
}

uninstallPackage() {
	[ $# != 1 ] && return
	local package=$1
	apt-get remove --yes --purge "$package" &>> "$LOG" || true
}
