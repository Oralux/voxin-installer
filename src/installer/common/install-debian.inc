debianIsPackageInstalled() {
	[ $# != 1 ] && return 1
	local deb=$1
	local status=0
	grep -A1 "Package: $deb$" /var/lib/dpkg/status | grep "Status: install ok installed" &> /dev/null || status=1
	return $status
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
	local voxinDeb=$(getVoxinDebPackage)
	local libvoxinTarball=$(getLibvoxinTarball)

	if [ ! -d "$rfsdir" ]; then
		echo "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
    apt-get -q update >> "$LOG"
	status=$?
	if [ "$status" != 0 ]; then
		echo "apt-get update failure!" >> "$LOG"
		return "$status"
	fi

	if [ -z "$voxinDeb" ]; then
		echo "voxin deb not found!" >> "$LOG"
		return 1
	fi
	
	if [ -z "$libvoxinTarball" ]; then
		echo "libvoxin pkg not found!" >> "$LOG"
		return 1
	fi

	for i in libvoxin1 voxind; do
		apt-get remove --yes --purge $i &>> "$LOG" || true
	done
	
	dpkg -i "$voxinDeb" &>> "$LOG"
	status=$?
	# TODO
	ls -l /usr/lib/libvoxin*
	if [ "$status" != 0 ]; then
		local oldSymlink="/usr/lib/libibmeci.so"
		if [ -e "$oldSymlink" ]; then
			echo "delete old symlink: $oldSymlink" >> "$LOG"
			mv "$oldSymlink" "$oldSymlink.orig"
			dpkg -i "$voxinDeb" &>> "$LOG"
			status=$?
		fi
		if [ "$status" != 0 ]; then
			echo "Can't install $voxinDeb!" >> "$LOG"
			return "$status"
		fi
	fi

	tar -C "$rfsdir" -xf "$libvoxinTarball" 
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: untar failure ($libvoxinTarball)" >> "$LOG"
	fi	
	
	return "$status"
}

uninstallSystem() {	
    apt-get remove --yes --purge libvoxin1 &>> "$LOG"
}

installSystem_sd() {
    return 0
}

orcaConf() {
    ls /home/*/.orca/user-settings.py 2>>"$LOG" | while read i; do
		sed -i "s/espeak/ibmtts/g" "$i"
    done

    ls /home/*/.orca/user-settings.pyc 2>>"$LOG" | while read i; do
		rm "$i"
    done
}

isSpeechDispatcherAvailable() {
	local status=0
	debianIsPackageInstalled speech-dispatcher || status=1
	return $status
}

isSpeechDispatcherVoxinInstalled() {
	local status=0
	debianIsPackageInstalled speech-dispatcher-voxin || debianIsPackageInstalled speech-dispatcher-ibmtts || status=1
	return $status
}

sd_install() {
	[ $# != 1 ] && return 1
	
	local rfsdir=$1
	local deb=$(getSpeechDispatcherDebPackage)
	[ -z "$deb" ] && return 1
	
	dpkg -i "$deb" &>> "$LOG" || return 1
	
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

uninstallLang() {
    return 0
}

installLang() {
	[ $# != 1 ] && return 1
	local rfsdir=$1
	local status
	local i

    askLicense || return 1

	i=$(getViavoiceAllTarball)
	if [ -z "$i" ]; then
		echo "Error: no viavoice-all tarball" >> "$LOG"
		return 1
	fi
	tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: untar failure ($i)" >> "$LOG"
		return $status
	fi
	
	getViavoiceTarballs | while read i; do
		tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			echo "Error: untar failure ($i)" >> "$LOG"
			return $status
		fi
	done

	postInstViavoiceTarball "$rfsdir" "/"
	
    return 0
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
