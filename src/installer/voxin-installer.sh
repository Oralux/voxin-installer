#!/bin/bash -x
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

BASE=$(dirname $(readlink -f $0))
cd $BASE

source ./common/init.inc
source ./common/spdconf.inc

_gettext() {
    if [ -n "$GETTEXT" ]; then
		echo; gettext "$1"
    else
		echo "$1"
    fi
}

getArch

unset GETTEXT
which gettext >> "$LOG"
if [ "$?" = "0" ]; then
    init_gettext "$BASE"/locale
    GETTEXT=0
fi

[ "$UID" != "0" ] && _gettext "Please run voxin-installer as root. " && exit 0

check_distro
if [ "$?" != "0" ]; then
	_gettext "Sorry, this distribution is not yet supported. "
	_gettext "For support, email to contact at oralux.org "
    exit 1
fi


# beta1
if [ -h /opt ]; then
	_gettext "Sorry, this installer does not expect a symbolic link /opt"
	_gettext "For support, email to contact at oralux.org "
    exit 1
fi
unset status
for i in libvoxin libvoxin1 voxind speech-dispatcher-voxin speech-dispatcher-ibmtts; do
	isPackageInstalled $i 
	if [ $? = 0 ]; then
		status="$status $i"
		break
	fi
done

if [ -n "$status" ]; then
	_gettext "Voxin Packages: $status"
	_gettext "Sorry, this beta version can't update a previous voxin installation"
	_gettext "For support, email to contact at oralux.org "
    exit 1
fi

with_sd=1
check_speech_dispatcher_voxin
if [ "$?" != "0" ]; then
	askContinue
	if [ $? = 1 ]; then
		_gettext "Good Bye. "
		exit 0
	fi
	with_sd=0
fi
			
TEMP=`getopt -o hlsuv --long help,lang,sd,uninstall,verbose -- "$@"`
if [ $? != 0 ] ; then
    usage
    exit 1
fi

eval set -- "$TEMP"

with_silent=0
with_lang=0
with_uninstall=0
with_verbose=0

while true ; do
    case "$1" in
		-l|--lang) with_silent=1; with_lang=1; shift;;
		-u|--uninstall) with_uninstall=1; shift;;
		-v|--verbose) with_verbose=1; shift;;
		--) shift ; break;;
		*) usage; exit 1;;
    esac
done

if [ "$with_verbose" = "1" ]; then
    set -x
fi

if [ "$with_silent" = "1" ]; then
    source common/askSilent.inc
else
    source common/ask.inc
fi

if [ -z "$GETTEXT" ]; then
    install_gettext
    init_gettext "$(pwd)/locale"
    GETTEXT=0
fi

if [ "$with_uninstall" = "1" ]; then
    uninstall
    exit 0
fi

_gettext "Log file: $LOG"
_gettext "Initialization; please wait... "
installDir=/
installSystem "$installDir" || exit 1

installed=0
askInstallLang && {
    installLang "$installDir" || exit 1
    installed=1

    unset PLAY OPT 
    PLAY=$(which paplay 2>/dev/null)
    if [ -n "$PLAY" ]; then
	OPT="--rate=11025 --channels=1 --format=s16le" 
    else
	PLAY=$(which aplay 2>/dev/null)
	if [ -n "$PLAY" ]; then
	    OPT="-r 11025 -c 1 -f S16_LE"
	fi
    fi

    export LD_LIBRARY_PATH="$installDir"/opt/oralux/voxin/lib
    if [ -n "$PLAY" ]; then
	# $OPT can usually be omitted (needed during the test on Gentoo)
	"$installDir"/opt/oralux/voxin/bin/voxin-say "Voxin: OK" | "$PLAY" $OPT &>> "$LOG"
    fi
}

[ "$with_sd" = 1 ] && spd_conf_set voxin

if [ "$installed" = "0" ]; then
    askUninstall && {
		uninstall
		exit 0
    }

fi

_gettext "The changes will be taken into account on next boot. "
_gettext "Good Bye. "
