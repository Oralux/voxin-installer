
archGetSuffix() {
    SUFFIX32=i686.pkg.tar.xz
    if [ "$ARCH" = amd64 ]; then
		SUFFIX=x86_64.pkg.tar.xz
    else
		SUFFIX=$SUFFIX32
    fi    
}

# TODO
archInstallDeb() {
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
	local voxinDeb=$(getVoxinDebPackage)
	local libvoxinTarball=$(getLibvoxinTarball)

	if [ ! -d "$rfsdir" ]; then
		echo "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
	pacman -Syu
	status=$?
	if [ "$status" != 0 ]; then
		echo "pacman update failure!" >> "$LOG"
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
		pacman --noconfirm -R $i &>> "$LOG" || true
	done
	
	archInstallDeb "$voxinDeb" &>> "$LOG"
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

uninstallSystem()
{
    pacman --noconfirm -R libvoxin1 &>>"$LOG"
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

isSpeechDispatcherVoxinInstalled() {
	local status=1
	local a=$(getPackageVersion speech-dispatcher-voxin)
	[ -n "$a" ] && status=0
	return $status
}

sd_install()
{
    installSystem_sd || return 1    
    archGetSuffix
	pacman -Q speech-dispatcher-ibmtts 2>/dev/null && pacman -Rs speech-dispatcher-ibmtts
    pacman -U --noconfirm "$Packages"/speech-dispatcher-voxin-*-"$SUFFIX" &>>"$LOG" || return 1
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

uninstallLang()
{
    return 0
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
    
#    cd "$BASE"
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

