rpmGetVoxinPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/rpm -name "libvoxin*.$ARCH.rpm"
}

getPackageVersion() {
	[ $# != 1 ] && return
	local deb=$1
	dnf list installed "$deb" | awk "/$deb/{print \$2}"	
}

installSystem()
{
	[ $# != 1 ] && return 1
	local rfsdir=$1
	local status=0
	local voxinPackage=$(rpmGetVoxinPackage)
	local rfsTarball=$(getRootfilesystemTarball)

	if [ ! -d "$rfsdir" ]; then
		echo "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
	if [ -z "$voxinPackage" ]; then
		echo "voxin package not found!" >> "$LOG"
		return 1
	fi
	
	if [ -z "$rfsTarball" ]; then
		echo "rfs tarball not found!" >> "$LOG"
		return 1
	fi

	tar -C "$rfsdir" -xf "$rfsTarball" 
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: untar failure ($rfsTarball)" >> "$LOG"
		return "$status"
	fi	

	dnf install -q -y "$voxinPackage" &>> "$LOG"	
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: package install failure ($voxinPackage)!" >> "$LOG"
		return "$status"
	fi
	
	return "$status"
}

uninstallSystem()
{	
    dnf remove -y libvoxin1 &>> "$LOG"	
}

isSpeechDispatcherAvailable()
{
	dnf list installed speech-dispatcher &>> "$LOG"	
}

isSpeechDispatcherVoxinInstalled() {
	dnf list installed speech-dispatcher-voxin &>> "$LOG"	
}

sd_install()
{
	[ $# != 1 ] && return 1
	
	local rfsdir=$1
	local package=$(getSpeechDispatcherPackage)
	[ -z "$package" ] && return 1
	
	dnf install -q -y "$package" &>> "$LOG"	|| return 1
	
    spd_conf_set ibmtts
    return 0
}

sd_uninstall()
{
    spd_conf_unset ibmtts
    spd_conf_set espeak
    dnf remove speech-dispatcher-voxin
    return 0
}

install_gettext()
{
    dnf install -q -y install gettext &>> "$LOG"
    . gettext.sh
}

getArch() {
    ARCH="$(uname -m)"
}

getSpeechDispatcherPackage() {
	local list=$(find packages/rpm -name "speech-dispatcher-voxin*.$ARCH.rpm")
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
# input: name1-name2-1.2.3-1.x86_64.rpm
# output: 1.2
getPackageFileVersion() {
	[ $# != 1 ] && return
	local package=$1
	getArch
	echo "$package" | sed -E -e "s/.$ARCH.pkg.tar.xz//" -e "s/.*-([[:digit:]]+.[[:digit:]]+).*/\1/" 
}

