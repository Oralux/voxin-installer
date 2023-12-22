LOG="$BASE/log/voxin.log"
TEMPLATE=voxin-installer.XXXXXXXXXX
TMPDIR=/tmp
# SPEECHD_MIN_VERSION: the minimal required version of
# speech-dispatcher provided by the system
if [ "$SYSTEM_WIDE_INSTALL" = 1 ]; then 
    SPEECHD_MIN_VERSION=0.7
else
    SPEECHD_MIN_VERSION=0.11
fi
VOXIN_VERSION=%VERSION%

unset WITH_TTS
ls $BASE/packages/all/voxin-viavoice-all* &> /dev/null && WITH_TTS=viavoice
ls $BASE/packages/all/voxin-nve-all* &> /dev/null && WITH_TTS=nve
case "$WITH_TTS" in
	viavoice) postInstTarball=postInstViavoiceTarball;;
	*) postInstTarball=noOp;;
esac




rm -f "$LOG"

unset MAJOR
unset MINOR
unset MACRO

leave() {
	echo -e "$1"
	exit $2
}

check_bash() {
    TAR_NO_OVERWRITE="--no-overwrite-dir"
    if [ "$(readlink /bin/bash)" = busybox ]; then
	unset TAR_NO_OVERWRITE
    fi
}

init_gettext() {
    export TEXTDOMAINDIR="$1"
    export TEXTDOMAIN=voxin
}

yes() {
    local a
    read a
    [ -z "$a" ]
    return $?
}

ok() {
    local status=1
    local a
	echo -n " "
    read a
    case "$a" in [oO][kK]*) status=0;; esac
	if [ "$status" = "0" ]; then		
		_gettext "Please wait..."
	else
		_gettext "You do not authorize the installer to perform this operation"
	fi	
    return $status
}

usage () {
    cat >&2 <<END
Usage: voxin-installer.sh [options]

Examples:
- System-wide install
  Launch as superuser:
  ./voxin-installer.sh

- Local install
  Launch as regular user:
  ./voxin-installer.sh

  installation under $DEFAULT_TOPDIR_USER.

- Local install under custom directory
  Launch as regular user or superuser:
  ./voxin-installer.sh -d pathname

Options:
     -a, --arch       set the target architecture (either x86_64, armv7l or aarch64)
     -d, --directory  install directory
     -h, --help       print this message
     -l, --lang       install the language automatically
     -u, --uninstall  uninstall the TTS
     -v, --verbose    verbose
END
}

uninstall() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local rfsdir="$1"
    spd_conf_unset voxin	
    uninstallSystem "$rfsdir"
    if [ $? != 0 ]; then
	_gettext "Error: operation not completed."
	exit 1
    fi
    
    if [ "$SYSTEM_WIDE_INSTALL" != 1 ]; then
	local dir1="$HOME/.local/share/voxin"
	local dir2=$(realpath "$dir1")
	askRemoveDir "$dir2" && { _gettext "removing install dir:" >> "$LOG"; echo "$dir2" >> $LOG; rm -rf "$dir2"; [ -d "$dir1" ] && rmdir "$dir1"; }
    fi

    _gettext "Operation completed. "
    return 0
}

check_distro() {
    if [ "$SYSTEM_WIDE_INSTALL" != 1 ]; then
	source common/install-tarballs.inc
	return 0
    fi
    
    local status=0
    local ID=$(awk -F= '/^ID_LIKE=/{print $2}' /etc/os-release | tr -d '"')
    [ -z "$ID" ] && ID=$(awk -F= '/^ID=/{print $2}' /etc/os-release | tr -d '"')
    [ -z "$ID" ] && return 1
    
    status=0
    case "$ID" in
	arch*) source common/install-arch-linux.inc;;
	debian*|ubuntu*) source common/install-deb.inc;;
	fedora*) source common/install-rpm-dnf.inc;;
	gentoo*) source common/install-gentoo.inc;;
	opensuse*|suse*) source common/install-rpm-zypper.inc;;
	rhel*) source common/install-rpm-yum.inc;;
	slackware*|slint*) source common/install-slackware.inc;;
	*) status=1;;
    esac

    check_bash
    
    return $status
}

# return 0 if speech-dispatcher is installed and then set SPEECHD_VER
# to the version of speech-dispatcher (or unset if no version
# identified)
# 
get_speech_dispatcher_version() {
    local status=1
    if [ -e /usr/bin/speech-dispatcher ]; then
	status=0
	SPEECHD_VER=$(LC_ALL=C /usr/bin/speech-dispatcher -v | head -1 | cut -f2 -d" ")
    fi
    return $status
}

# if speech-dispatcher is already installed, check if this installer
# provides the compatible voxin output module
check_speech_dispatcher_voxin() {
    get_speech_dispatcher_version
    if [ $? = 0 ]; then
	#	local installedVersion=$(getPackageVersion speech-dispatcher)
	# if no version identified, suppose compatibility acquired
	[ -z "$SPEECHD_VER" ] && return 0

	isVersionGreater "$SPEECHD_VER" "$SPEECHD_MIN_VERSION"
    fi
}

# getTarballVersion
# Example:
# input=name_1.2.3.x86_64.txz
# output=1.2.3
getTarballVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	echo "$1" | sed 's+.*_\(.*\)\..*\..*+\1+'
}

# getMajMinVersion
# Example:
# input=1.2.3-4.fc29
# output=1.2
getMajMinVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	echo "$1" | cut -f1,2 -d.
}

getRootfilesystemAllTarball() {
    local tarball=$(find packages/all -name "rfs*.all.txz")

    if [ -z "$tarball" ]; then
	_gettext "rfs all tarball not found!" >> "$LOG"
	return 1
    fi
    echo "$tarball"
    return 0
}

getRootfilesystemArchTarball() {
    local tarball=$(find packages/all -name "rfs*.$ARCH.txz")

    if [ -z "$tarball" ]; then
	_gettext "rfs all tarball not found!" >> "$LOG"
	return 1
    fi
    echo "$tarball"
    return 0
}

getPrevSpeechdModulesTarball() {
    local tarball=$(find packages/all -name "sd_voxin_*.$ARCH.txz")

    if [ -z "$tarball" ]; then
#	_gettext "prev speechd modules tarball not found!" >> "$LOG"
	return 1
    fi
    echo "$tarball"
    return 0
}

noOp() {
	return 0
}

getVoiceAllTarball() {
	find packages/all -name "voxin-*-all_*.txz"
}

getVoiceTarball() {
	find packages/all -name "voxin-*txz" ! -name "voxin-*-all_*.txz"
}

getVoxindTarball() {
	find packages/all -name "voxind-*.$ARCH.txz"
}

postInstViavoiceTarball() {
	[ $# != 3 ] && return 1
	[ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] && return 1

	local rfsdir="$1"
	local destdir="$2"
	local newconf="$3"
	local inidir="$rfsdir/opt/IBM/ibmtts/etc"
	local rfs32="$rfsdir/opt/oralux/voxin/rfs32"
	local LANG
	local i

	if [ ! -f "$inidir/all.ini" ]; then
		_gettext "Notice: no $inidir/all.ini"
		return 0
	fi

	# get list of installed languages
	LANG=$(find "$inidir/../lib" -regex ".*/...50.so" | sed "s+.*/\(.*\)50.so+\1+")

	if [ -z "$LANG" ]; then
		_gettext "No language found in $inidir/../lib"
		return 1
	fi

	cp "$inidir/all.ini" "$newconf" 
	if [ $? != 0 ]; then
		_gettext "Write error: $inidir/all.ini"
		return 1
	fi

	for i in $LANG; do
		if [ -e "$inidir/$i.ini" ]; then
			cat "$inidir/$i.ini" >> "$newconf"
			if [ $? != 0 ]; then
				_gettext "Write error: $inidir/all.ini"
				return 1
			fi
		else
			_gettext "Notice: no $inidir/$i.ini"		
			return 1
		fi
	done

	sed -i "s#=/opt/#=$destdir/opt/#" "$newconf"
	if [ $? != 0 ]; then
		_gettext "Write error: $newconf"
		return 1
	fi

	return 0
}

installVoxinNve() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	local langdir=opt/oralux/nve/share/vop2/languages
	local start="<datapackagename>"
	local end="</datapackagename>"
	local tarball=$(ls packages/all/voxin-ve-*.txz)
	[ -f "$tarball" ] || { _gettext "[ivn] Error: no voxin-ve tarball"; return 1; }

	local line=$(tar -f "$tarball" --wildcards -x "$langdir/*/speech/ve/*.hdr" -O | grep "$start")  || { _gettext "[ivn] Error: extract voxin-ve: $i"; return 1; }
	echo "line=$line" >> "$LOG"

	line=$(echo "$line" | grep -Po "(?<=$start)(.*)(?=$end)")
	echo "line=$line" >> "$LOG"

	[ -n "$line" ] || { _gettext "[ivn] Error: no start/end found"; return 1; }

	read -r lang name quality ver< <(echo "$line" | sed 's|/| |g')
	echo "lang=$lang name=$name quality=$quality ver=$ver" >> "$LOG"
	[ -n "$lang" ] && [ -n "$name" ] && [ -n "$quality" ] && [ -n "$ver" ] || { _gettext "[ivn] Error: ve fields not found"; return 1; }

	IFS=$(echo -en "\n\b")
	# temporary backup of the previously installed voice
	local tmp_dat=$(ls "$rfsdir"/"$langdir"/"$lang"/speech/components/"$lang"_"$name"_"$quality"_*.dat  2>/dev/null)
	local tmp_hdr=$(ls "$rfsdir"/"$langdir"/"$lang"/speech/ve/ve_pipeline_"$lang"_"$name"_22_"$quality"_*.hdr  2>/dev/null)
	local i
	local dir
	local previous_voice
	[ -n "$tmp_hdr" ] && previous_voice=1
	if [ -n "$previous_voice" ]; then
		for i in $tmp_dat $tmp_hdr; do
			# temporary backup of the previously installed voice 
			[ -f "$i" ] || { _gettext "[ivn] Error: unexpected entry: $i"; return 1; }
			cp -a "$i" "$i.tmp" || { dir=$(dirname "$i"); rm "$dir"/*.tmp; _gettext "[ivn] Error: failure to backup file ($i)"; return 1; }
		done

		# suppress the previously installed voice (having the same language, name and quality as the voice to install)
		for i in $tmp_hdr $tmp_dat; do
			rm "$i" || { _gettext "[ivn] Error: failure to remove file: $i"; return 1; }
		done
	fi

	# install the voice tarball
	getVoiceTarball | while read i; do
		tar -C "$rfsdir" $TAR_NO_OVERWRITE -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			_gettext "[ivn] Error: untar failure ($i)" >> "$LOG"
			return $status
		fi
	done

	if [ -n "$previous_voice" ]; then
		# remove the temporary backup
		for i in $tmp_hdr $tmp_dat; do
			dir=$(dirname "$i"); rm "$dir"/*.tmp 2>/dev/null || :
		done
	fi

	return 0
}

installLang() {
	echo installLang >> "$LOG"
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	local status
	local i

	"$askLicense" || return 1

	i=$(getVoiceAllTarball)

	if [ -z "$i" ]; then
		_gettext "Error: no 'all' tarball" >> "$LOG"
		return 1
	fi
	tar -C "$rfsdir" $TAR_NO_OVERWRITE -xf "$i"
	status=$?
	if [ "$status" != 0 ]; then
		_gettext "Error: untar failure ($i)" >> "$LOG"
		return $status
	fi

	if [ "$WITH_TTS" = nve ]; then
		i=$(getVoxindTarball)
		if [ -z "$i" ]; then
			_gettext "Error: no 'voxind' tarball" >> "$LOG"
			return 1
		fi
		tar -C "$rfsdir" $TAR_NO_OVERWRITE -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			_gettext "Error: untar failure ($i)" >> "$LOG"
			return $status
		fi

		local OIFS=$IFS
		installVoxinNve "$rfsdir"
		IFS=$OIFS

		status=$?
	elif [ "$WITH_TTS" = viavoice ]; then
		# install the voice tarball
 		getVoiceTarball | while read i; do
			tar -C "$rfsdir" $TAR_NO_OVERWRITE -xf "$i"
			status=$?
			if [ "$status" != 0 ]; then
				_gettext "Error: untar failure ($i)" >> "$LOG"
				return $status
			fi
		done
		local ECI_INI="$TOPDIR/var/opt/IBM/ibmtts/cfg/eci.ini"
		"$postInstTarball" "$rfsdir" "$TOPDIR" "$ECI_INI"
	fi
	
	return 0
}

getArch() {
    if [ $# = 1 ] && [ -n "$1" ]; then
	ARCH="$1"
    else
	ARCH=$(uname -m)
    fi
    PACMAN_ARCH=$ARCH
    case "$ARCH" in
	x86_64|ia64)
	    DEBIAN_ARCH=amd64
    	    ;;
	arm*)
	    DEBIAN_ARCH=armhf
	    PACMAN_ARCH=armv7h
	    ;;
	aarch64)	
	    # Check if the root filesystem is also in 64 bits
	    # Test: read byte @0x04 (EI_CLASS) of an ELF header
	    # if EI_CLASS == 1: 32 bits (if 2: 64 bits)
	    # Ref: https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html
	    #
	    DEBIAN_ARCH=arm64
	    EI_CLASS=$(hexdump -s4 -n1 -e '/1 "%02X"' /bin/bash)
	    if [ "$EI_CLASS" = "01" ]; then
		ARCH=armv7l
		DEBIAN_ARCH=armhf
		PACMAN_ARCH=armv7h
	    fi	    
	    ;;
	*)
	    DEBIAN_ARCH=i386
    	    ;;
    esac
}

uninstallSystem()
{	
	echo uninstallSystem >> "$LOG"
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	isPackageInstalled voxin && { uninstallPackage voxin || return 1; }
	isPackageInstalled voxin-speechd && { uninstallPackage voxin-speechd || return 1; }
	isPackageInstalled voxin-speechd-conf && { uninstallPackage voxin-speechd-conf || return 1; }

	if [ "$SYSTEM_WIDE_INSTALL" != 1 ]; then
	    spd_uninstall_local_conf
	    uninstallLocalSpeechdModule
	fi

	for i in "$rfsdir/opt/oralux/voxin" "$rfsdir/var/opt/oralux/voxin" "$rfsdir/opt/IBM/ibmtts" "$rfsdir/var/opt/IBM/ibmtts" "$rfsdir/opt/oralux/nve"; do
		rm -rf $i
	done
	for i in "$rfsdir/opt/oralux" "$rfsdir/var/opt/oralux" "$rfsdir/opt/IBM" "$rfsdir/var/opt/IBM"; do
		[ -e "$i" ] && [ -z "$(ls -A "$i")" ] && rm -rf "$i"
	done
	return 0
}

is_voxin_conf_installed() {
    ls -lL "$TOPDIR"/etc/speech-dispatcher/modules/voxin.conf &> /dev/null
}

installSystem() {	
	echo installSystem >> "$LOG"
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	local status=0
	local voxinPackage=$(getVoxinPackage)
	local voxinSpeechdPackage=$(getVoxinSpeechdPackage)
	local voxinSpeechdConfPackage=$(getVoxinSpeechdConfPackage)
	local rfsAllTarball=$(getRootfilesystemAllTarball) || return 1
	local rfsArchTarball=$(getRootfilesystemArchTarball) || return 1
	local prevSpeechdModulesTarball=$(getPrevSpeechdModulesTarball) || unset prevSpeechdModulesTarball
	local voxin_conf_installed=0
	local sd_voxin_installed=0

	if [ ! -d "$rfsdir" ]; then
		_gettext "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
	if [ -z "$voxinPackage" ]; then
		_gettext "voxin package not found!" >> "$LOG"
		return 1
	fi
	
	if [ -z "$voxinSpeechdPackage" ]; then
		_gettext "voxin-speechd package not found!" >> "$LOG"
		return 1
	fi

	if [ -z "$voxinSpeechdConfPackage" ]; then
		_gettext "voxin-speechd-conf package not found!" >> "$LOG"
		return 1
	fi

	for i in voxin voxin-speechd voxin-speechd-conf; do
	    isPackageInstalled $i && { uninstallPackage $i || return 1; }
	done

	is_sd_voxin_installed && sd_voxin_installed=1
	is_voxin_conf_installed && voxin_conf_installed=1
	    
	local i
	for i in $rfsAllTarball $rfsArchTarball $prevSpeechdModulesTarball; do
	    [ -z "$i" ] && continue
	    tar -C "$rfsdir" -xf "$i" 
	    status=$?
	    if [ "$status" != 0 ]; then
		_gettext "Error: untar failure ($i)" >> "$LOG"
		return "$status"
	    fi	
	done

	# install voxin.ini globally
	local ref_voxin_ini="$TOPDIR"/opt/oralux/voxin/share/conf/voxin.ini
	local global_voxin_ini="$TOPDIR"/var/opt/oralux/voxin/voxin.ini	
	[ ! -e $global_voxin_ini ] && install -m 644 -D $ref_voxin_ini $global_voxin_ini

	# install voxin package
	installLocalPackage "$voxinPackage"
	status=$?
	[ "$status" != 0 ] && { _gettext "Error: package install failure ($voxinPackage)!" >> "$LOG"; \
				[ "$(id -u)" = 0 ] && _gettext "Check if it helps to become root using 'su --login' or 'sudo --login'" >> "$LOG"; \
				return $status; }

	# install voxin.conf (if not yet installed)
	if [ "$voxin_conf_installed" = 0 ]; then
	    local conf_dir="$TOPDIR"/opt/oralux/voxin/share/speech-dispatcher
	    local conf="$conf_dir/0.10.1/voxin.conf"
	    spd_conf_install_voxin_conf "$conf"

	    # install voxin-speechd-conf package
	    installLocalPackage "$voxinSpeechdConfPackage"
	fi

	# install sd_voxin (if not yet provided by the distro)
	if [ "$sd_voxin_installed" = 0 ]; then
	    # install voxin-speechd package
	    installLocalPackage "$voxinSpeechdPackage"
	fi

	if [ "$SYSTEM_WIDE_INSTALL" != 1 ]; then
	    spd_install_local_conf
	    installLocalSpeechdModule
	fi
	
	postInstSystem

	return "$status"
}

install_gettext() {
    installPackage gettext
    . gettext.sh
}

# getMajorMinorMicro
# The result is supplied in $MAJOR, $MINOR, $MICRO
#
# MAJOR.MINOR.MICRO
# MAJOR: n digits
# MINOR: n digits
# MICRO: n digits (optionally followed by non digits characters which will be filtered)
# e.g.
# 0.7.1_abce
# gives major=0, minor=7, micro=1
#
# Return 0 if ok, 1 otherwise
getMajorMinorMicro() {
	[ $# != 1 ] && return 1
	local version=$1
	local major
	local minor
	local micro

	unset MAJOR
	unset MINOR
	unset MICRO
	
	major=$(echo "$version" | sed -r "s/([0-9]+)\.[0-9]+.*/\1/")	
	[ "$major" = "$version" ] && return 1

	minor=$(echo "$version" | sed -r "s/[0-9]+\.([0-9]+).*/\1/")	
	[ "$minor" = "$version" ] && return 1
	
	micro=$(echo "$version" | sed -r "s/[0-9]+\.[0-9]+\.([0-9]+).*/\1/")
	[ "$micro" = "$version" ] && micro=0

	MAJOR=$major
	MINOR=$minor
	MICRO=$micro
	
	return 0
}

# compare version1 to version2
#
# RETURN
# 0: ver1 >= ver2
# 1: ver1 < ver2
# 2: ver1 with unexpected format (see getMajorMinorMicro)
# 3: ver2 with unexpected format
isVersionGreater() {
	[ $# != 2 ] && return 1
	local ver1=$1
	local ver2=$2
	local major
	local minor
	local micro

	getMajorMinorMicro "$ver1" || return 2
	major1=$MAJOR
	minor1=$MINOR
	micro1=$MICRO

	getMajorMinorMicro

	[ "$major1" -gt "$MAJOR" ] && return 0
	[ "$major1" -lt "$MAJOR" ] && return 1

	[ "$minor1" -gt "$MINOR" ] && return 0
	[ "$minor1" -lt "$MINOR" ] && return 1
	
	[ "$micro1" -gt "$MICRO" ] && return 0
	[ "$micro1" -lt "$MICRO" ] && return 1
		
	return 0
}

