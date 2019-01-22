#!/bin/bash -x
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

BASE=$(realpath dirname $0)
cd $BASE

source ./common/init.inc
source ./common/spdconf.inc

unset GETTEXT
which gettext >> "$LOG"
if [ "$?" = "0" ]; then
    init_gettext "$(pwd)/locale"
    GETTEXT=0
fi

if [ "$UID" != "0" ]; then
    if [ -n "$GETTEXT" ]; then
		echo; gettext "Please run voxin-installer as root. "
    else
		echo -n "Please run voxin-installer as root. "
    fi	
    exit 0
fi  

check_distro
if [ "$?" != "0" ]; then
    if [ -n "$GETTEXT" ]; then
		echo; gettext "Sorry, this distribution is not yet supported. "
    else
		echo -n "Sorry, this distribution is not yet supported. "
    fi
    exit 0
fi

check_speech_dispatcher_voxin
if [ "$?" != "0" ]; then
	echo; gettext "Sorry, this installer does not provide a compatible replacement version for speech-dispatcher-voxin."
    exit 0
fi
			
TEMP=`getopt -o hlsuv --long help,lang,sd,uninstall,verbose -- "$@"`
if [ $? != 0 ] ; then
    usage
    exit 1
fi

eval set -- "$TEMP"

with_silent=0
with_lang=0
with_sd=0
with_uninstall=0
with_verbose=0

while true ; do
    case "$1" in
		-l|--lang) with_silent=1; with_lang=1; shift;;
		-s|--sd) with_silent=1; with_sd=1; shift;;
		-u|--uninstall) with_uninstall=1; shift;;
		-v|--verbose) with_verbose=1; shift;;
		--) shift ; break;;
		*) usage; exit 1;;
    esac
done

if [ "$with_verbose" = "1" ]; then
    set -x
fi

getArch

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

echo; gettext "Log file: $LOG"
echo; gettext "Initialization; please wait... "
installDir=/
installSystem "$installDir" || exit 1

installed=0
askInstallLang && {
    installLang "$installDir" || exit 1
    installed=1

	unset PLAY
	for i in aplay paplay; do
		PLAY=$(which $i 2>/dev/null) && break
	done

	export LD_LIBRARY_PATH="$installDir"/opt/oralux/voxin/lib
	if [ -n "$PLAY" ]; then
		"$installDir"/opt/oralux/voxin/bin/voxin-say "Voxin: OK" | $PLAY &>> "$LOG"
	fi
}

isSpeechDispatcherAvailable && askInstallSpeechDriver && {
		sd_install "$installDir" || exit 1
		installed=1
		orcaConf
    }

if [ "$installed" = "0" ]; then
    askUninstall && {
		uninstall
		exit 0
    }

    echo; gettext "The changes will be taken into account on next boot. "
fi

echo; gettext "Good Bye. "
