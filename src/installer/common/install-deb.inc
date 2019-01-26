debianGetVoxinPackage() {
	[ -z "$DEBIAN_ARCH" ] && return 1
	find packages/deb -name "libvoxin*_$DEBIAN_ARCH.deb"
}

debianIsPackageInstalled() {
	[ $# != 1 ] && return 1
	local deb=$1
	grep -A1 "Package: $deb$" /var/lib/dpkg/status | grep "Status: install ok installed" &> /dev/null
}

getPackageVersion() {
	[ $# != 1 ] && return
	local deb=$1
	dpkg-query --showformat='${Version}' --show "$deb"
}

installSystem() {	
	[ $# != 1 ] && return 1
	local rfsdir=$1
	local status=0
	local voxinPackage=$(debianGetVoxinPackage)
	local rfsTarball=$(getRootfilesystemTarball)

	if [ ! -d "$rfsdir" ]; then
		echo "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
    # apt-get -q update >> "$LOG"
	# status=$?
	# if [ "$status" != 0 ]; then
	# 	echo "apt-get update failure!" >> "$LOG"
	# 	return "$status"
	# fi

	if [ -z "$voxinPackage" ]; then
		echo "voxin package not found!" >> "$LOG"
		return 1
	fi
	
	if [ -z "$rfsTarball" ]; then
		echo "rfs tarball not found!" >> "$LOG"
		return 1
	fi

	for i in libvoxin1 voxind; do
		apt-get remove --yes --purge $i &>> "$LOG" || true
	done
	
	tar -C "$rfsdir" -xf "$rfsTarball" 
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: untar failure ($rfsTarball)" >> "$LOG"
		return "$status"
	fi	

	dpkg -i "$voxinPackage" &>> "$LOG"
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: package install failure ($voxinPackage)!" >> "$LOG"
		return "$status"
	fi
	
	return "$status"
}

uninstallSystem() {	
    apt-get remove --yes --purge libvoxin1 &>> "$LOG"
}

isSpeechDispatcherAvailable() {
	debianIsPackageInstalled speech-dispatcher
}

isSpeechDispatcherVoxinInstalled() {
	debianIsPackageInstalled speech-dispatcher-voxin || debianIsPackageInstalled speech-dispatcher-ibmtts
}

sd_install() {
	[ $# != 1 ] && return 1
	
	local rfsdir=$1
	local package=$(getSpeechDispatcherPackage)
	[ -z "$package" ] && return 1
	
	dpkg -i "$package" &>> "$LOG" || return 1
	
    spd_conf_set ibmtts
    return 0
}

sd_uninstall() {
    spd_conf_unset ibmtts
    spd_conf_set espeak
    apt-get remove speech-dispatcher-voxin
    return 0
}

install_gettext() {
    apt-get -q install --yes gettext >> "$LOG"
    . gettext.sh
}

getArch() {
	ARCH=$(uname -m)
    case "$ARCH" in
		x86_64|ia64)
			DEBIAN_ARCH=amd64
    	    ;;
		*)
			DEBIAN_ARCH=i386
    	    ;;
    esac
}

getSpeechDispatcherPackage() {
	local list=$(find packages/deb -name "speech-dispatcher-voxin_*_$DEBIAN_ARCH.deb")
	local i
	local availableVersion
	local package
	local installedVersion=$(getPackageVersion speech-dispatcher)
	[ -z "installedVersion" ] && return
	
	if [ -n "$list" ]; then
		for i in $list; do
			availableVersion=$(getPackageFileVersion "$i")
			local ver1=$(getMajMinVersion $availableVersion)
			local ver2=$(getMajMinVersion $installedVersion)
			if [ "$ver1" = "$ver2" ]; then
				package=$i
				break
			fi
		done
	fi
	echo "$package"
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

