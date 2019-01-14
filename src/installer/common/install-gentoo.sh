#!/bin/bash -e
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

BUILD_DIR="$(mktemp -d)"
BASE="$PWD"

installSystem()
{	
    # apt-get -q update >> "$LOG"
    # if [ "$ARCH" = "amd64" ]; then
    # 	dpkg --print-foreign-architectures | grep -q i386
    # 	if [ "$?" != "0" ]; then
    # 	    dpkg --add-architecture i386
    # 	    apt-get -q update >> "$LOG"
    # 	fi
	
    # 	apt-get -q install --yes libc6:i386 >> "$LOG"
    # fi

    #32 bits
    push "$BUILD_DIR"
    ldconfig -p | grep -q libstdc++-libc6.2-2.so.3
    if [ "$?" == "1" ]; then
	mkdir libstdc++
	cd libstdc++
	ar -x "$BASE"/packages/all/libstdc++2.10-glibc2.2*.deb &>> "$LOG"
	tar -C / data.tar.xz &>> "$LOG"
    fi
    cd ..
    mkdir libvoxin
    cd libvoxin
    ar -x "$BASE"/packages/all/libvoxin1_*_"$ARCH".deb &>> "$LOG"
    tar -C / data.tar.xz &>> "$LOG"

    cd ..
    mkdir voxind
    cd voxind
    ar -x "$BASE"/packages/all/voxind_*_i386.deb &>> "$LOG"
    tar -C / data.tar.xz &>> "$LOG"
    
}


uninstallSystem()
{	
    apt-get remove --yes --purge libvoxin1 voxind 
#    apt-get remove libstdc++2.10-glibc2.2:i386
}


installSystem_sd()
{
    return 0
}

orcaConf()
{
    ls /home/*/.orca/user-settings.py 2>>"$LOG") | while read i; do
		sed -i "s/espeak/ibmtts/g" "$i"
    done

    ls /home/*/.orca/user-settings.pyc 2>>"$LOG") | while read i; do
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
    
    dpkg -i "$Packages"/speech-dispatcher-voxin_*_"$ARCH".deb &>> "$LOG"
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

isSpeechDispatcherVoxinInstalled() {
	return 0
}
