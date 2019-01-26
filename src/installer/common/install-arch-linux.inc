
archGetVoxinPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/archlinux -name "libvoxin*$ARCH.pkg.tar.xz"
}

getPackageVersion() {
	[ $# != 1 ] && return
	local deb=$1
	pacman -Q "$deb" 2>/dev/null | awk '{print $2}'
}

installSystem() {	
	[ $# != 1 ] && return 1
	local rfsdir=$1
	local status=0
	local voxinPackage=$(archGetVoxinPackage)
	local rfsTarball=$(getRootfilesystemTarball)

	if [ ! -d "$rfsdir" ]; then
		echo "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
#	pacman -Syu
	# status=$?
	# if [ "$status" != 0 ]; then
	# 	echo "pacman update failure!" >> "$LOG"
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
		pacman --noconfirm -R $i &>> "$LOG" || true
	done
	
	tar -C "$rfsdir" -xf "$rfsTarball" 
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: untar failure ($rfsTarball)" >> "$LOG"
		return "$status"
	fi	

	pacman -U --noconfirm "$voxinPackage" &>>"$LOG"
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: package install failure ($voxinPackage)!" >> "$LOG"
		return "$status"
	fi

	return "$status"
}

uninstallSystem()
{
    pacman --noconfirm -R libvoxin1 &>>"$LOG"
}

isSpeechDispatcherAvailable()
{
	pacman -Q speech-dispatcher &>/dev/null
}

isSpeechDispatcherVoxinInstalled() {
	local status=1
	local a=$(getPackageVersion speech-dispatcher-voxin)
	[ -n "$a" ] && status=0
	return $status
}

sd_install()
{
	[ $# != 1 ] && return 1
	
	local rfsdir=$1
	local package=$(getSpeechDispatcherPackage)
	[ -z "$package" ] && return 1
	
    pacman -U --noconfirm "$package" &>>"$LOG" || return 1
	
    spd_conf_set ibmtts
    return 0	
}

sd_uninstall()
{
    spd_conf_unset ibmtts
    spd_conf_set espeak
    pacman --noconfirm -R speech-dispatcher-voxin
    return 0
}

install_gettext()
{
    pacman -S --noconfirm gettext &>>"$LOG" || return 1
    . gettext.sh
}

getArch() {
    ARCH=$(uname -m)
}

getSpeechDispatcherPackage() {
	local list=$(find packages/archlinux -name "speech-dispatcher-voxin*$ARCH.pkg.tar.xz")
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
# input: name1-name2-1.2.3-1-x86_64.pkg.tar.xz
# output: 1.2
getPackageFileVersion() {
	[ $# != 1 ] && return
	local package=$1
	getArch
	echo "$package" | sed -E -e "s/-$ARCH.pkg.tar.xz//" -e "s/.*-([[:digit:]]+.[[:digit:]]+).*/\1/" 
}

