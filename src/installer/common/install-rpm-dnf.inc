getVoxinPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/rpm -name "libvoxin*.$ARCH.rpm"
}

getPackageVersion() {
	[ $# != 1 ] && return
	local deb=$1
	dnf list installed "$deb" | awk "/$deb/{print \$2}"	
}

isPackageInstalled() {
	[ $# != 1 ] && return 1
	local deb=$1
	dnf list installed "$deb" &>/dev/null
}

# getPackageFileVersion
# Example:
# input: name1-name2-1.2.3-1.x86_64.rpm
# output: 1.2
getPackageFileVersion() {
	[ $# != 1 ] && return
	local package=$1
	getArch
	echo "$package" | sed -E -e "s/.$ARCH.pkg.tar.xz//" -e "s/.*-([[:digit:]]+.[[:digit:]]+).*/\1/" 
}

# getListSpeechDispatcherVoxin
# output: 2 vars
# getListSpeechDispatcherVoxin1: list of sd_ibmtts packages
# getListSpeechDispatcherVoxin2: list of ibmtts.conf packages
getListSpeechDispatcherVoxin() {
	getListSpeechDispatcherVoxin1=$(find packages/rpm -name "speech-dispatcher-voxin*.$ARCH.rpm" ! -name "speech-dispatcher-voxin-conf*")
	getListSpeechDispatcherVoxin2=$(find packages/rpm -name "speech-dispatcher-voxin-conf*.$ARCH.rpm")
}

installLocalPackage() {
	[ $# != 1 ] && return
	local package=$1
	dnf install -q -y "$package" &>> "$LOG"
}

installPackage() {
	installLocalPackage $1
}

uninstallPackage() {
	[ $# != 1 ] && return
	local package=$1
    dnf remove -y "$package" &>> "$LOG"	
}
