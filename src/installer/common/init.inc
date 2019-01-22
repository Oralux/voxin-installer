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
    uninstallLang
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
		source common/install-fedora.inc
		identify_fedora
		status=$?
    else
		# Check if this is a debian based distro
		a=$(which dpkg) || true
		if [ -n "$a" ]; then
			source common/install-debian.inc
			status=0
		fi
    fi
    return $status
}

# if the speech-dispatcher-voxin package is already installed, check
# if this installer provides a compatible replacement version
check_speech_dispatcher_voxin() {
    local status=0
    local deb
	$(isSpeechDispatcherVoxinInstalled) || return 0
	deb=$(getSpeechDispatcherDebPackage)
	[ -z "$deb" ] && status=1
	return $status
}

merge_old_archive() {
    local archive="$1"
}

# getTarballVersion
# Example:
# input=name_1.2.3.x86_64.txz
# output=1.2.3
getTarballVersion() {
	[ $# != 1 ] && return
	echo "$1" | sed 's+.*_\(.*\)\..*\..*+\1+'
}

# getMajMinVersion
# Example:
# input=1.2.3
# output=1.2
getMajMinVersion() {
	[ $# != 1 ] && return
	echo "$1" | sed 's/\(.*\..*\)\..*/\1/'
}

# getDebPackageVersion
# Example:
# input: name_1.2.3-5voxin1_amd64.deb
# output: 1.2.3
getDebPackageVersion() {
	[ $# != 1 ] && return
	local deb=$1
	echo "$deb" | sed 's+.*_\(.*\)-.*+\1+'
}

getSpeechDispatcherDebPackage() {
	local list=$(find packages/debian -name "speech-dispatcher-voxin_*_$DEBIAN_ARCH.deb")
	local i
	local availableVersion
	local deb
	local installedVersion=$(getPackageVersion speech-dispatcher)
	[ -z "installedVersion" ] && return
	
	if [ -n "$list" ]; then
		for i in $list; do
			availableVersion=$(getDebPackageVersion "$i")
			local ver1=$(getMajMinVersion $availableVersion)
			local ver2=$(getMajMinVersion $installedVersion)
			if [ "$ver1" = "$ver2" ]; then
				deb=$i
				break
			fi
		done
	fi
	echo "$deb"
}

getVoxinDebPackage() {
	find packages/debian -name "libvoxin*_$DEBIAN_ARCH.deb"
}

getLibvoxinTarball() {
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
