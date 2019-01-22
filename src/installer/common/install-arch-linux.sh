#!/bin/bash
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

LIBVOXIN=libvoxin.so.1

#####
BASE=$PWD
Distro="archlinux"
Packages="$BASE/packages/$Distro"
RFS=/opt/voxin

pacman -Syu

getPackageVersion() {
# TODO
	[ $# != 1 ] && return 1
	local deb=$1
#	dpkg-query --showformat='${Version}' --show "$deb"
}


# remove the multilib section if any
removeMultilibSection() {
    start="$(grep -n '^\[multilib\]' "$RFS"/pacman.conf | cut -f1 -d: )"
    stop="$(wc -l "$RFS"/pacman.conf | cut -f1 -d" " )"

    if [ -n "$start" ]; then
		stop1="$(grep -n '^\[' "$RFS"/pacman.conf | cut -f1 -d: | \
		       while read x; do \
			   if [ "$x" -gt "$start" ]; then \
			       echo "$x"; \
			       break; \
			   fi; \
		       done; \
	     )"
		if [ -n "$stop1" ]; then	
			stop=$(($stop1-1))
		fi
    fi
    if [ -n "$start" ] && [ -n "$stop" ]; then 
		sed -i "$start,$stop d" "$RFS"/pacman.conf
    fi    
}

# Create a minimal 32 bits rootfs in /opt/voxin
# Derived from
# https://wiki.archlinux.org/index.php/Install_bundled_32-bit_system_in_64-bit_system
#
buildRFS() {
    [ -d "$RFS" ] && return 1

    echo; gettext "Installing a 32 bits system in /opt/voxin..."
    
    mkdir -p "$RFS"/var/{cache/pacman/pkg,lib/pacman}
    sed -e "s/\$arch/i686/g" /etc/pacman.d/mirrorlist > "$RFS"/mirrorlist
    sed -e "s@/etc/pacman.d/mirrorlist@$RFS/mirrorlist@g" -e "/Architecture/ s,auto,i686,"  /etc/pacman.conf > "$RFS"/pacman.conf
    removeMultilibSection
    pacman --noconfirm --root "$RFS" --cachedir "$RFS"/var/cache/pacman/pkg --config "$RFS"/pacman.conf -Sy \
		   2>>"$LOG" 1>&2 || return 1

    LIST="glibc"
    for p in $LIST; do
		echo; gettext "Installing package: "; echo -n $p
		pacman --noconfirm --root "$RFS" --cachedir "$RFS"/var/cache/pacman/pkg --config "$RFS"/pacman.conf -S "$p" \
     	       2>>"$LOG" 1>&2 || return 1
    done    
}

installLibCPP() {
    LIBSTD=libstdc++2.10-glibc2.2_2.95.4-27_i386
    pacman --noconfirm --root "$RFS" --cachedir "$RFS"/var/cache/pacman/pkg --config "$RFS"/pacman.conf -U "$Packages"/$LIBSTD*i686.pkg.tar.xz \
		   2>>"$LOG" 1>&2 || return 1       
}

getSuffix() {
    SUFFIX32=i686.pkg.tar.xz
    if [ "$ARCH" = amd64 ]; then
		SUFFIX=x86_64.pkg.tar.xz
    else
		SUFFIX=$SUFFIX32
    fi    
}

installLibvoxin() {
    getSuffix

    pacman --noconfirm --root "$RFS" --cachedir "$RFS"/var/cache/pacman/pkg --config "$RFS"/pacman.conf -U "$Packages"/voxind*$SUFFIX32 \
		   2>>"$LOG" 1>&2 || return 1

    LIBVOXIN="$(ls "$Packages"/libvoxin*$SUFFIX 2>/dev/null)"
    pacman -U --noconfirm --force "$LIBVOXIN" 2>>"$LOG" 1>&2 || return 1
}

installSystem()
{
    buildRFS || return 1
    installLibCPP       
}

uninstallSystem()
{
    pacman --noconfirm -R voxin
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
    getSuffix
	pacman -Q speech-dispatcher-ibmtts 2>/dev/null && pacman -Rs speech-dispatcher-ibmtts
    pacman -U --noconfirm "$Packages"/speech-dispatcher-voxin-*-"$SUFFIX" \
    	   2>>"$LOG" 1>&2 || return 1
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
    pacman -S --noconfirm gettext \
    	   2>>"$LOG" 1>&2 || return 1
    . gettext.sh
}

installLang()
{
    askLicense || return 1

    [ -d "$RFS/tmp" ] || return 1


    # mount -o bind /dev "$RFS"/dev
    # mount -o bind /dev/pts "$RFS"/dev/pts
    # mount -t proc none "$RFS"/proc
    # mount -t sysfs none "$RFS"/sys
    
    cp -a mnt "$RFS"/tmp/
    cd mnt
    
    install -d 0755 "$RFS"/opt/IBM "$RFS"/var/opt/IBM

    # equivalent of the previous install.sh (which does not need a shell interpreter in the chroot)
    Control=rte/control
    Data=rte/data    
    if [ -e "$RFS"/opt/IBM/ibmtts/bin/inifilter ]; then
		# $Control/prerm
		cmd="$(grep opt "$Control"/prerm)"
		eval chroot "$RFS" "$cmd" 
    fi
    "$Control"/preinst
    # inst
    tar -C "$Data" -cf - . | tar -C "$RFS" --no-overwrite-dir -xf -
    
    #$Control/postinst 1
    # symlinks for chroot
    for i in libetidev.so libibmeci.so; do
		ln -sf /opt/IBM/ibmtts/lib/"$i" "$RFS"/usr/lib/
    done

    grep opt rte/control/postinst | grep -v ln | while read cmd; do eval chroot "$RFS" "$cmd"; done

    ### lang ###    
    Control=lang/control
    Data=lang/data
    # inst
    tar -C "$Data" -cf - . | tar -C "$RFS" --no-overwrite-dir -xf -
    #$Control/postinst 1
    grep opt lang/control/postinst | while read cmd; do eval chroot "$RFS" "$cmd"; done

    chroot "$RFS" ldconfig /usr/lib    

    #####    
    installLibvoxin

    for i in / /var/; do
		mkdir -p ${i}opt/IBM
		ln -sf /opt/voxin${i}opt/IBM/ibmtts ${i}opt/IBM
    done

    # for d in sys proc /dev/pts /dev; do
    # 	umount "$RFS"/$d
    # done
    
    cd "$BASE"
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

isSpeechDispatcherVoxinInstalled() {
	return 0
}
