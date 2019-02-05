LOG="$BASE/log/voxin.log"
TEMPLATE=voxin-installer.XXXXXXXXXX
TMPDIR=/tmp

rm -f "$LOG"

leave() {
	echo -e "$1"
	exit $2
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
    read a
    case "$a" in [oO][kK]) status=0;; esac
    return $status
}

usage () {
    cat >&2 <<END
Usage: voxin-installer.sh [options]
Options:
     -h, --help       print this message
     -l, --lang       install the language
     -s, --sd         install the speech dispatcher driver
     -u, --uninstall  uninstall the TTS
     -v, --verbose    verbose
END
}

uninstall() {
    sd_uninstall
    uninstallSystem
    echo; gettext "Operation completed. "    
}

check_distro() {
    local status=1
    local ID="$(awk -F= '/ID/{print $2}' /etc/os-release)"

    # if [ "$ID" = "gentoo" ]; then
	# 	source common/install-gentoo.inc
	# 	return $?
    # fi

    local a=$(which pacman) || true
    if [ -n "$a" ]; then
		source common/install-arch-linux.inc
		status=$?
    elif [ -e "/etc/fedora-release" ]; then 
		source common/install-rpm-dnf.inc
		status=$?
	elif [ -e "/etc/slackware-version" ]; then 
		source common/install-slackware.inc
		status=$?
    else
		# Check if this is a debian based distro
		a=$(which dpkg) || true
		if [ -n "$a" ]; then
			source common/install-deb.inc
			status=0
		fi
    fi
    return $status
}

# if the speech-dispatcher-voxin package is already installed, check
# if this installer provides a compatible replacement version
check_speech_dispatcher_voxin() {
	local installedVersion
	$(isSpeechDispatcherVoxinInstalled) || return 0
	installedVersion=$(getPackageVersion speech-dispatcher)
	getListSpeechDispatcherVoxin || return 1
	getLocalSpeechDispatcherVoxinPackage "$installedVersion" "$output"	
}

merge_old_archive() {
    local archive="$1"
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

getRootfilesystemTarball() {
	find packages/all -name "rfs*.$ARCH.txz"
}

getViavoiceAllTarball() {
	find packages/all -name "voxin-viavoice-all*.txz"
}

getViavoiceTarballs() {
	find packages/all -name "voxin-viavoice*.txz" ! -name "voxin-viavoice-all*.txz"
}

postInstViavoiceTarball() {
	[ $# != 2 ] && return 1
	[ -z "$1" ] || [ -z "$2" ] && return 1

	local rfsdir=$1
	local destdir=$2
	local inidir=$rfsdir/opt/IBM/ibmtts/etc
	local newconf=$rfsdir/var/opt/IBM/ibmtts/cfg/eci.ini
	local rfs32=$rfsdir/opt/oralux/voxin/rfs32
	local LANG
	local i

	if [ ! -f "$inidir/all.ini" ]; then
		echo "Notice: no $inidir/all.ini"
		return 0
	fi

	# get list of installed languages
	LANG=$(find "$inidir/../lib" -regex ".*/...50.so" | sed "s+.*/\(.*\)50.so+\1+")

	if [ -z "$LANG" ]; then
		echo "No language found in $inidir/../lib"
		return 1
	fi

	cp "$inidir/all.ini" "$newconf" 
	if [ $? != 0 ]; then
		echo "Write error: $inidir/all.ini"
		return 1
	fi

	for i in $LANG; do
		if [ -e "$inidir/$i.ini" ]; then
			cat "$inidir/$i.ini" >> "$newconf"
			if [ $? != 0 ]; then
				echo "Write error: $inidir/all.ini"
				return 1
			fi
		else
			echo "Notice: no $inidir/$i.ini"		
			return 1
		fi
	done

	sed -i "s#=/opt/#=$destdir/opt/#" "$newconf"
	if [ $? != 0 ]; then
		echo "Write error: $newconf"
		return 1
	fi

	return 0
}

installLang() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
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

orcaConf() {
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

sd_install() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	
	local rfsdir=$1
	local binPackage
	local confPackage
	local installedVersion=$(getPackageVersion speech-dispatcher)
	[ -z "$installedVersion" ] && return 1
	
	getListSpeechDispatcherVoxin || return 1
	getLocalSpeechDispatcherVoxinPackage "$installedVersion" "$output" || return 1
	binPackage=$output
	
	getListSpeechDispatcherVoxinConf || return 1
	getLocalSpeechDispatcherVoxinPackage "$installedVersion" "$output" || return 1
	confPackage=$output
		
	installLocalPackage "$binPackage" || return 1

	# if the original system does not already provide ibmtts.conf,
	# then install our local package
	local installConf=0
	isPackageInstalled speech-dispatcher-voxin-conf
	if [ "$?" = 1 ]; then
		if [ -e /usr/share/speech-dispatcher/conf/modules/ibmtts.conf ] || [ -e /etc/speech-dispatcher/modules/ibmtts.conf ]; then
			installConf=1
		fi
	fi
	if [ "$installConf" = 0 ]; then
		installLocalPackage "$confPackage" || return 1
	fi
    spd_conf_set ibmtts
    return 0
}

sd_uninstall()
{
    spd_conf_unset ibmtts
    spd_conf_set espeak
	uninstallPackage speech-dispatcher-voxin
    return 0
}

uninstallSystem()
{	
	uninstallPackage libvoxin1
}

installSystem() {	
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir=$1
	local status=0
	local voxinPackage=$(getVoxinPackage)
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

	for i in libvoxin1 voxind; do
		uninstallPackage $i || true
	done
	
	tar -C "$rfsdir" -xf "$rfsTarball" 
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: untar failure ($rfsTarball)" >> "$LOG"
		return "$status"
	fi	

	installLocalPackage "$voxinPackage"
	status=$?
	if [ "$status" != 0 ]; then
		echo "Error: package install failure ($voxinPackage)!" >> "$LOG"
		return "$status"
	fi
	
	return "$status"
}

isSpeechDispatcherAvailable() {
	isPackageInstalled speech-dispatcher
}

isSpeechDispatcherVoxinInstalled() {
	isPackageInstalled speech-dispatcher-voxin || isPackageInstalled speech-dispatcher-ibmtts
}

# output: pathname of the local speech-dispatcher-voxin(-conf) package
# compatible with the installed speech-dispatcher
getLocalSpeechDispatcherVoxinPackage() {	
	unset output
	[ $# != 2 ] && return 1
	[ -z "$1" ] || [ -z "$2" ] && return 1
	local installedVersion=$1	
	local localPackages=$2
	local i
	
	for i in $localPackages; do
		getPackageFileVersion "$i"
		local ver1=$(getMajMinVersion "$output")
		local ver2=$(getMajMinVersion "$installedVersion")
		if [ "$ver1" = "$ver2" ]; then
			output=$i
			break
		fi
		unset output
	done
	[ -n "$output" ]
}

install_gettext() {
    installPackage gettext
    . gettext.sh
}

