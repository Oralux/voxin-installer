#!/bin/bash -vx
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#


#####

identify_fedora() 
{
    local status=1
    DISTRIB_ID="$(awk -F"=" '/^ID=/{print $2}' /etc/os-release)"
    DISTRIB_RELEASE="$(awk -F'=' '/^VERSION_ID=/{print $2}' /etc/os-release)"
    
    case "$DISTRIB_ID" in
		"fedora")
			case "$DISTRIB_RELEASE" in
				#            25|26) status=0;;			
				*) DISTRIB_RELEASE=latest;;
			esac
			;;
    esac
    return $status
}

installSystem()
{
    dnf install -q -y packages/all/voxin-*"$ARCH".rpm &>> "$LOG"
}

uninstallSystem()
{	
    dnf remove -y voxin
}

installSystem_sd()
{
    return 0
}

orcaConf()
{
    return 0
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
    
    dnf install -q -y "$Packages"/speech-dispatcher-voxin-*."$ARCH".rpm &>> "$LOG"
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

uninstallLang()
{
    return 0
}

installLang()
{
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
    ARCH="$(uname -m)"
}

isSpeechDispatcherVoxinInstalled() {
	return 0
}
