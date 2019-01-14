#!/bin/bash
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

LOG="$PWD/log/voxin.log"
rm -f "$LOG"


leave() {
	echo -e "$1"
	exit $2
}

init_gettext()
{
    export TEXTDOMAINDIR="$1"
    export TEXTDOMAIN=voxin
}

yes()
{
    local a
    read a
    [ -z "$a" ]
    return $?
}

ok()
{
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

uninstall()
{
    uninstallLang || exit 1
    sd_uninstall
    uninstallSystem
    echo; gettext "Operation completed. "    
}

check_distro()
{
    local status=1
    local ID="$(awk -F= '/ID/{print $2}' /etc/os-release)"

    # if [ "$ID" = "gentoo" ]; then
	# 	source common/install-gentoo.sh
	# 	return $?
    # fi

    type pacman &>> "$LOG"
    if [ "$?" = "0" ]; then
		source common/install-arch-linux.sh
		status=$?
    elif [ -e "/etc/fedora-release" ]; then 
		source common/install-fedora.sh
		identify_fedora
		status=$?
    else
		# Check if this is a debian based distro
		type dpkg &>> "$LOG"
		if [ "$?" = "0" ]; then
			source common/install-debian.sh
			identify_debian
			status=$?
		fi
    fi
    return $status
}

# if the speech-dispatcher-voxin package is already installed, check
# if this installer provides a compatible replacement version
check_speech_dispatcher_voxin() {
	local status=0
	$(isSpeechDispatcherVoxinInstalled)
	if [ $? = 0 ]; then 
		local deb=$(getSpeechDispatcherDebPackage)
		[ -z "$deb" ] && status=1
	fi	
	return $status
}

merge_old_archive()
{
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
	local list=$(ls packages/all/$arch/speech-dispatcher-voxin_*.deb 2>/dev/null)
	local i
	local availableVersion
	local deb
	local sd=$(which speech-dispatcher 2>/dev/null)

	[ -z "$sd" ] && return

	local installedVersion=$($sd -v | head -n1| cut -f2 -d" ")
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
	ls packages/all/$arch/voxin-pkg_*.deb 2>/dev/null
}

getLibvoxinTarball() {
	ls packages/all/$arch/libvoxin_*.txz 2>/dev/null
}

getViavoiceAllTarball() {
	ls packages/all/$arch/voxin-viavoice-all_*.txz 2>/dev/null
}

getViavoiceTarballs() {
	ls packages/all/$arch/voxin-viavoice_*.txz 2>/dev/null
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

