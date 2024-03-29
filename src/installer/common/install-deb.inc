getVoxinPackage() {
	[ -z "$DEBIAN_ARCH" ] && return 1
	find packages/deb -name "voxin_*_$DEBIAN_ARCH.deb"	
}

getVoxinSpeechdPackage() {
	[ -z "$DEBIAN_ARCH" ] && return 1
	find packages/deb -name "voxin-speechd_*$DEBIAN_ARCH.deb"	
}

getVoxinSpeechdConfPackage() {
	[ -z "$DEBIAN_ARCH" ] && return 1
	find packages/deb -name "voxin-speechd-conf_*$DEBIAN_ARCH.deb"	
}

is_sd_voxin_installed() {
    { dpkg -L speech-dispatcher-voxin | grep sd_voxin || { dpkg -L speech-dispatcher | grep sd_voxin; }; } &> /dev/null
    return 1
}

getPackageVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local deb=$1
	dpkg-query --showformat='${Version}' --show "$deb" 2>/dev/null
}

isPackageInstalled() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local deb=$1
	grep -A1 "Package: $deb$" /var/lib/dpkg/status | grep "Status: install ok installed" &> /dev/null
}

# getPackageFileVersion
# Example:
# input: name_1.2.3-5voxin1_amd64.deb
# output: 1.2
getPackageFileVersion() {
	unset output
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	output=$(echo "$package" | sed -E -e "s/_$DEBIAN_ARCH.deb//" -e "s/.*_([[:digit:]]+.[[:digit:]]+).*/\1/")
}

installLocalPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	dpkg -i "$package" &>> "$LOG"
}

installPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
    apt-get -q install --yes "$package" &>> "$LOG"
}

uninstallPackage() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local package=$1
    apt-get remove --yes --purge "$package" &>> "$LOG"
}

postInstSystem() {
    return 0
}
