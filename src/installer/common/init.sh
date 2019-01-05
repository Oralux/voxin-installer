#!/bin/bash
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

LOG="$PWD/log/voxin.log"
rm -f "$LOG"

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

    if [ "$ID" = "gentoo" ]; then
	source common/install-gentoo.sh
	return $?
    fi

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

merge_old_archive()
{
    local archive="$1"
}

check_runtime()
{
    local status=0
    local archive
    local dir
    if [ ! -d "mnt/rte" ]; then
	status=1
	echo; gettext "Sorry, the IBM TTS runtime is not included in this update."
	echo; gettext "If you already bought an older voxin archive then type its path below."
	echo; gettext "Example of path: /home/user1/voxin-enu-0.43.tgz"
	while [ 1 ]; do
	    echo ; gettext "Enter path (or Control C to quit): "
	    read -e archive
	    if [ -f "$archive" ]; then
		(tar -ztf "$archive" | grep -e ".*/.*/mnt/rte") &>> $LOG
		if [ "$?" = "0" ]; then
		    dir="$(tar -ztf "$archive" | grep -e ".*/.*/mnt/$")"
		    tar -zxf "$archive" "$dir" --strip-components=2
		    status=$?
		    break;
		fi
	    fi
	done
    fi
    return $status
}
