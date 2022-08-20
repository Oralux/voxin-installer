#!/bin/bash
# This file is under the LGPL license
# 2007-2022, Gilles Casse <gcasse@oralux.org>
#

BASE=$(dirname $(readlink -f $0))
cd $BASE

source ./common/init.inc

if [ "$(readlink /bin/bash)" = busybox ]; then
    source ./common/spdconffake.inc
else
    source ./common/spdconf.inc
fi

DEFAULT_TOPDIR_SYSTEM_WIDE=/
DEFAULT_TOPDIR_USER="$HOME/.local/share/voxin/rfs"
VOXDIR=opt/oralux/voxin

NEW_VER=$VOXIN_VERSION
NEWDIR=opt/oralux/voxin.$NEW_VER

SPEECHD_MODULE_DIR_USER="$HOME/.local/libexec/speech-dispatcher-modules"

_gettext() {
    if [ -n "$GETTEXT" ]; then
		echo | tee -a "$LOG"; gettext "$1" | tee -a "$LOG"
    else
		echo "$1" | tee -a "$LOG"
    fi
}

exit_on_error() {
    _gettext "Error: more details in $LOG"
    _gettext "For support, email to contact at oralux.org "
    exit 1
}

say_ok() {
    local voxdir=$1
    unset PLAY OPT
    PLAY=$(which aplay 2>/dev/null)
    if [ -z "$PLAY" ]; then
	PLAY=$(which paplay 2>/dev/null)
    fi

    export LD_LIBRARY_PATH="$voxdir/lib"
    if [ -n "$PLAY" ]; then
	"$voxdir/bin/voxin-say" "Voxin: OK" | "$PLAY" >> "$LOG" 2>&1
    fi    
}

goodbye() {
    _gettext "The changes will be taken into account on next boot. "
    _gettext "Good Bye. "
    exit 0
}

select_version() {
    local NEW_VER=$1

    # 1. check current version
    [ -z "$OLD_VER" ] && { _gettext "Error: no compatible version of voxin installed"; exit_on_error; }
    
    # 2. unselect the current version
    local DIR="$TOPDIR/$VOXDIR"
    if [ -h "$DIR" ]; then
	unlink "$DIR" || _gettext "Error: can't unlink $DIR" && exit_on_error
    elif [ -d "$DIR" ]; then
	[ -e "$DIR.$OLD_VER" ] && { _gettext "Error: if applicable remove $DIR.$OLD_VER"; exit_on_error; }
	mv "$DIR" "$DIR.$OLD_VER" || { _gettext "Error: can't move $DIR to $DIR.$OLD_VER"; exit_on_error; }
    fi

    [ ! -d "$DIR.$OLD_VER" ] && { _gettext "Error: can't found directory $DIR.$OLD_VER"; exit_on_error; }
    
    # 3. select the new version as current version
    ln -sf "$DIR.$NEW_VER" "$DIR"
    if [ $? != 0 ]; then
	_gettext "Error: can't link $DIR.$NEW_VER to $DIR"
	_gettext "Attempting to restore previous version $OLD_VER"
	#    || _gettext "select_version: error, can't link $DIR.$NEW_VER $DIR" && exit_on_error
	ln -sf "$DIR.$OLD_VER" "$DIR" || _gettext "select_version: error, can't link $DIR.$OLD_VER to $DIR" && exit_on_error
	_gettext "Previous version restored" && exit_on_error
    fi
}

update_version() {
    source common/askSilent.inc

    if [ -z "$GETTEXT" ]; then
	install_gettext
	init_gettext "$(pwd)/locale"
	GETTEXT=0
    fi
    
    [ -z "$OLD_VER" ] && { _gettext "Error: no compatible version of voxin installed"; exit_on_error; }
    [ "$NEW_VER" = "$OLD_VER" ] && { _gettext "Error: version $NEW_VER already installed"; exit_on_error; }
    
    
#    installDir=/
    DESTDIR="$TOPDIR/$NEWDIR"
    [ -e "$DESTDIR" ] && { _gettext "error, if applicable, remove this directory: $DESTDIR"; exit_on_error; }

    tmpdir="$TOPDIR/$VOXDIR/../tmp.voxin.$NEW_VER"
    mkdir -p  "$tmpdir"
    [ -d "$tmpdir" ] || { _gettext "Can't create directory $tmpdir"; exit_on_error; }   
    updateSystem "$tmpdir" || { exit_on_error; }
    installLang "$tmpdir" || { exit_on_error; }

    mv "$tmpdir/$VOXDIR" "$DESTDIR"
    select_version "$NEW_VER"
    rm -rf "$tmpdir"
    say_ok "$TOPDIR/$VOXDIR"
    _gettext "Voxin updated to version $NEW_VER"
}

# Entry point

unset TOPDIR

unset GETTEXT
which gettext >> "$LOG"
if [ "$?" = "0" ]; then
    init_gettext "$BASE"/locale
    GETTEXT=0
fi

TEMP=`getopt -o a:d:hlLuUv --long arch,dir,help,lang,uninstall,update,verbose -- "$@"`
if [ $? != 0 ] ; then
    usage
    exit 1
fi

eval set -- "$TEMP"

with_silent=0
with_lang=0
with_update=0
with_uninstall=0
with_verbose=0
with_sd=0
unset with_arch
unset with_topdir

while true ; do   
    case "$1" in
	-a|--arch) with_arch=$2; shift 2;;
	-d|--dir) with_topdir=$2; shift 2;;
	-l|--lang) with_silent=1; with_lang=1; shift;;
	-U|--update) with_silent=1; with_update=1; shift;;
	-u|--uninstall) with_uninstall=1; shift;;
	-v|--verbose) with_verbose=1; shift;;
	--) shift ; break;;
	*) usage; exit 1;;
    esac
done

#[ "$UID" != "0" ] && _gettext "Please run voxin-installer as superuser. " && exit 0
SYSTEM_WIDE_INSTALL=$((UID==0?1:0))

if [ -n "$with_topdir" ]; then
    TOPDIR=$(realpath "$with_topdir")
    [ "$TOPDIR" = "/" ] && { usage; exit 1; }
    SYSTEM_WIDE_INSTALL=0
elif [ "$SYSTEM_WIDE_INSTALL" = 1 ]; then 
    TOPDIR="$DEFAULT_TOPDIR_SYSTEM_WIDE"
else
    TOPDIR="$DEFAULT_TOPDIR_USER"
fi

[ -n "$with_arch" ] && SYSTEM_WIDE_INSTALL=0

getArch "$with_arch"

check_distro
if [ "$?" != "0" ]; then
    _gettext "Sorry, this distribution is not yet supported. "
    _gettext "You may want to install voxin to a dedicated directory (option -d)"
    _gettext "For support, email to contact at oralux.org "
    exit 1	
fi

if [ ! -e "$TOPDIR" ]; then
    mkdir -p "$TOPDIR"
fi

if [ -h "$TOPDIR"/opt ]; then
	_gettext "Sorry, this installer does not expect a symbolic link $TOPDIR/opt"
	_gettext "For support, email to contact at oralux.org "
    exit 1
fi

OLD_INI="$TOPDIR/$VOXDIR"/share/doc/voxin-installer/sources.ini
[ -e "$OLD_INI" ] && OLD_VER=$(awk -F= '/tag/{print $2}' "$OLD_INI")

if [ "$with_update" = 1 ]; then
    update_version
    goodbye
fi

if [ "$with_uninstall" = 0 ]; then
	with_sd=1
	check_speech_dispatcher_voxin
	case $? in
		1)
			_gettext "Your version of speech-dispatcher is too old (required version >= $SPEECHD_MIN_VERSION)"
			_gettext "If the installation continues, orca will not be able to use voxin, but emacspeak will"
			_gettext "Do you really want to continue?"
			_gettext "If yes, press the ENTER key."
			yes
			if [ $? = 1 ]; then
				_gettext "Good Bye. "
				exit 0
			fi
			with_sd=0
			;;
		2)
			_gettext "Your version of speech-dispatcher has not been recognized."
			_gettext "If the installation continues, voxin will use its module for speech-dispatcher $SPEECHD_MAX_VERSION"
			_gettext "Do you really want to continue?"
			_gettext "If yes, press the ENTER key."
			yes
			if [ $? = 1 ]; then
				_gettext "Good Bye. "
				exit 0
			fi
			with_sd=0
	esac
fi

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
    askUninstall && uninstall "$TOPDIR"
    exit 0
fi

_gettext "Logs written in log/voxin.log"
_gettext "Initialization; please wait... "

# remove packages from voxin < 2.0
#for i in speech-dispatcher-ibmtts speech-dispatcher-voxin voxind libvoxin libvoxin1; do
for i in voxind libvoxin libvoxin1; do
	isPackageInstalled $i && uninstallPackage $i
done

uninstallOldVoxin "$TOPDIR" || { installOldVoxin "$TOPDIR"; exit_on_error; }
installSystem "$TOPDIR" || { installOldVoxin "$TOPDIR"; exit_on_error; }

installed=0
askInstallLang && {
    installLang "$TOPDIR" || { exit_on_error; }
    installed=1

    say_ok "$TOPDIR/$VOXDIR"
}

[ "$with_sd" = 1 ] && spd_conf_is_update_required && askUpdateConfAuthorization && spd_conf_set voxin

if [ "$installed" = "0" ]; then
    askUninstall && {
		uninstall "$TOPDIR" || exit_on_error
		exit 0
    }
fi

goodbye
