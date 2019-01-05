#!/bin/bash -e
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

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
    local Packages="packages/$DISTRIB_ID.$DISTRIB_RELEASE"

    apt-get -q update >> "$LOG"

	VOXIN=$(ls "$Packages"/voxin_*_$ARCH.deb 2>/dev/null)

	[ -z "$VOXIN" ] && VOXIN="$(ls packages/all/voxin_*_$ARCH.deb)"

    dpkg -i "$VOXIN" &>> "$LOG"

}


uninstallSystem()
{	
    apt-get remove --yes --purge voxin
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
    return 0
}

sd_install()
{
    installSystem_sd || return 1
    local Packages="packages/$DISTRIB_ID.$DISTRIB_RELEASE"
    if [ ! -e "$Packages" ]; then
		echo; gettext "Sorry, the speech dispatcher packages are not yet included in this archive."
		return 1
    fi
    
    dpkg -i "$Packages"/speech-dispatcher-voxin_*_$ARCH.deb &>> "$LOG"
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
    if [ "$TERM" = "dumb" ];
    then
		LESS=cat
		CLEAR=
    else
		LESS="less -e"
		CLEAR=clear
    fi

    askLicense || return 1

    cd mnt
    # if [ "$ARCH" = "amd64" ]; then
    # 	export LD_LIBRARY_PATH=/usr/lib
    # fi
    
    # preserve directory metadata
    install -d 0755 /opt/IBM /var/opt/IBM
    sed -i 's+tar -C / -oxf -+tar -C / --no-overwrite-dir -xf -+' install.sh 
    ./install.sh || return 1
    
    cd ..
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
