#!/bin/bash -e
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

debianIsPackageInstalled() {
	[ $# != 1 ] && return
	local deb=$1
	grep -A1 "Package: $deb$" /var/lib/dpkg/status | grep "Status: install ok installed" &> /dev/null
}

identify_debian() 
{
    local status=1
	local Packages
    
    DISTRIB_ID="$(awk -F"=" '/^ID=/{print $2}' /etc/os-release)"
    DISTRIB_RELEASE="$(awk -F'"' '/^VERSION_ID=/{print $2}' /etc/os-release)"
    
    case "$DISTRIB_ID" in
		"ubuntu")
			status=0
			case "$DISTRIB_RELEASE" in
#				"14.04"|"15.10"|"16.04"|"16.10"|"17.04") ;;
				*) DISTRIB_RELEASE=latest;;
			esac
			;;
		"debian")
			status=0
			case "$DISTRIB_RELEASE" in
#				"8"|"9") ;;
				*) DISTRIB_RELEASE=latest;;
			esac
			;;
		"kali")
			status=0
			case "$DISTRIB_RELEASE" in
				# 201*)
				# 	status=0
				# 	# interactive mode required (ask to restart)
				# 	LOG=/proc/self/fd/1
				# 	Packages="packages/$DISTRIB_ID.$DISTRIB_RELEASE"
				# 	if [ ! -e "$Packages" ]; then
				# 		DISTRIB_RELEASE=latest
				# 	fi
				# 	;;
				*) DISTRIB_RELEASE=latest;;
			esac
			;;
    esac
    return $status
}

installSystem()
{	
	[ $# != 1 ] && return 1
	local rfsdir=$1
	local status=0
	local voxinDeb=$(getVoxinDebPackage)
	local libvoxinTarball=$(getLibvoxinTarball)

	if [ ! -d "$rfsdir"]; then
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
		apt-get remove --yes --purge $i &>> "$LOG"
	done
	
	dpkg -i "$voxinDeb" &>> "$LOG"
	status=$?
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


uninstallSystem()
{	
    apt-get remove --yes --purge voxin-pkg &>> "$LOG"
}


installSystem_sd()
{
    return 0
}

orcaConf()
{
    ls /home/*/.orca/user-settings.py 2>>"$LOG" | while read i; do
		sed -i "s/espeak/ibmtts/g" "$i"
    done

    ls /home/*/.orca/user-settings.pyc 2>>"$LOG" | while read i; do
		rm "$i"
    done
}

isSpeechDispatcherAvailable()
{
	debianIsPackageInstalled speech-dispatcher
}

isSpeechDispatcherVoxinInstalled() {
	debianIsPackageInstalled speech-dispatcher-voxin || debianIsPackageInstalled speech-dispatcher-ibmtts
}

sd_install()
{
	[ $# != 1 ] && return 1
	
	local rfsdir=$1
	local deb=$(getSpeechDispatcherDebPackage)
	[ -z "$deb" ] && return 1
	
	dpkg -i "$deb" &>> "$LOG" || return 1
	
    spd_conf_set ibmtts
    return 0
}

sd_uninstall()
{
    spd_conf_unset ibmtts
    spd_conf_set espeak
    apt-get remove speech-dispatcher-voxin
    return 0
}

install_gettext()
{
    apt-get -q install --yes gettext >> "$LOG"
    . gettext.sh
}


uninstallLang()
{	
    rm -rf /opt/IBM/ibmtts
    rm -rf /var/opt/IBM/ibmtts
}

installLang()
{
	[ $# != 1 ] && return 1
	local rfsdir=$1
	local status

    askLicense || return 1

	getViavoiceTarballs | while read i; do
		tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			echo "Error: untar failure ($libvoxinTarball)" >> "$LOG"
			return $status
		fi
	done

    return 0
}

getArch() {
    case "$(uname -m)" in
		x86_64|ia64)
			ARCH=amd64
    	    ;;
		*)
			ARCH=i386
    	    ;;
    esac
}
