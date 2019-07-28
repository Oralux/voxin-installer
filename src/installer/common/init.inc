LOG="$BASE/log/voxin.log"
TEMPLATE=voxin-installer.XXXXXXXXXX
TMPDIR=/tmp
# Only version > 0.7 are compatible
#SPEECHD_MIN_VERSION=0.7
# TODO 3.0rc1 only version > 0.8.5
SPEECHD_MIN_VERSION=0.8.5
VOXIN_VERSION=%VERSION%

unset WITH_TTS
ls $BASE/packages/all/voxin-viavoice-all* &> /dev/null && WITH_TTS=viavoice
ls $BASE/packages/all/voxin-nve-all* &> /dev/null && WITH_TTS=nve
case "$WITH_TTS" in
	viavoice) postInstTarball=postInstViavoiceTarball;;
	*) postInstTarball=noOp;;
esac




rm -f "$LOG"

unset outputArray
declare -a outputArray

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
     -u, --uninstall  uninstall the TTS
     -v, --verbose    verbose
END
}

uninstall() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local rfsdir="$1"
    uninstallSystem "$rfsdir"
    _gettext "Operation completed. "    
}

check_distro() {
    local status=0
	local ID=$(awk -F= '/^ID_LIKE=/{print $2}' /etc/os-release | tr -d '"')
	[ -z "$ID" ] && ID=$(awk -F= '/^ID=/{print $2}' /etc/os-release | tr -d '"')
	[ -z "$ID" ] && return 1

	status=0
	case "$ID" in
		arch*) source common/install-arch-linux.inc;;
		debian*) source common/install-deb.inc;;
		fedora*) source common/install-rpm-dnf.inc;;
		gentoo*) source common/install-gentoo.inc;;
		opensuse*|suse*) source common/install-rpm-zypper.inc;;
		rhel*) source common/install-rpm-yum.inc;;
		slackware*|slint*) source common/install-slackware.inc;;
		*) status=1;;
	esac
	
    return $status
}

# if speech-dispatcher is already installed, check if this installer
# provides the compatible voxin output module
check_speech_dispatcher_voxin() {
	local installedVersion=$(getPackageVersion speech-dispatcher)
	isVersionGreater "$installedVersion" "$SPEECHD_MIN_VERSION"
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
}

getRootfilesystemArchTarball() {
    local arch=$(uname -m)
    local tarball=$(find packages/all -name "rfs*.$arch.txz")

    if [ -z "$tarball" ]; then
	_gettext "rfs all tarball not found!" >> "$LOG"
	return 1
    fi
    echo "$tarball"
}

noOp() {
	return 0
}

getVoiceAllTarball() {
	find packages/all -name "voxin-*-all*.txz"
}

getVoiceTarball() {
	find packages/all -name "voxin-*txz" ! -name "voxin-*-all*.txz"
}

getVoxindTarball() {
	find packages/all -name "voxind-*txz"
}

postInstViavoiceTarball() {
	[ $# != 2 ] && return 1
	[ -z "$1" ] || [ -z "$2" ] && return 1

	local rfsdir="$1"
	local destdir="$2"
	local inidir="$rfsdir/opt/IBM/ibmtts/etc"
	local newconf="$rfsdir/var/opt/IBM/ibmtts/cfg/eci.ini"
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

installLang() {
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
	tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
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
		tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			_gettext "Error: untar failure ($i)" >> "$LOG"
			return $status
		fi

		# TODO to be removed when libvoxin is able to select the nve engine
		local VOXIND_NVE="$rfsdir/opt/oralux/nve/bin/voxind-nve"
		local VOXIND="$rfsdir/opt/oralux/voxin/rfs32/usr/bin/voxind"
		[ ! -e "$VOXIND.orig" ] && cp "$VOXIND" "$VOXIND.orig"
		cp "$VOXIND_NVE" "$VOXIND"		
	fi
	
	getVoiceTarball | while read i; do
		tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			_gettext "Error: untar failure ($i)" >> "$LOG"
			return $status
		fi
	done

	"$postInstTarball" "$rfsdir" "/"
	
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

uninstallOldVoxin()
{	
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	uninstallPackage voxin
	DIR="$rfsdir/opt/oralux/voxin"
	if [ -e "$DIR" ]; then
		[ -e "$DIR.old" ] && rm -rf "$DIR.old"
		mv "$DIR" "$DIR.old"
	fi
}

uninstallSystem()
{	
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	uninstallPackage voxin
	for i in "$rfsdir/opt/oralux/voxin" "$rfsdir/opt/IBM/ibmtts" "$rfsdir/var/opt/IBM/ibmtts" "$rfsdir/opt/oralux/nve" ; do
		rm -rf $i
	done
	for i in "$rfsdir/opt/oralux" "$rfsdir/opt/IBM" "$rfsdir/var/opt/IBM"; do
		[ -z "$(ls -A $i)" ] && rm -rf $i
	done
}

installSystem() {	
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	local status=0
	local voxinPackage=$(getVoxinPackage)
	local rfsAllTarball=$(getRootfilesystemAllTarball) || return 1
	local rfsArchTarball=$(getRootfilesystemArchTarball) || return 1
	
	if [ ! -d "$rfsdir" ]; then
		_gettext "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
	if [ -z "$voxinPackage" ]; then
		_gettext "voxin package not found!" >> "$LOG"
		return 1
	fi
	
	uninstallPackage voxin || true

	local i
	for i in "$rfsAllTarball" "$rfsArchTarball"; do
	    tar -C "$rfsdir" -xf "$i" 
	    status=$?
	    if [ "$status" != 0 ]; then
		_gettext "Error: untar failure ($i)" >> "$LOG"
		return "$status"
	    fi	
	done
	
	installLocalPackage "$voxinPackage"
	status=$?
	if [ "$status" != 0 ]; then
		_gettext "Error: package install failure ($voxinPackage)!" >> "$LOG"
		return "$status"
	fi
	
	return "$status"
}

install_gettext() {
    installPackage gettext
    . gettext.sh
}

getMajorMinorMicro() {
	[ $# != 1 ] && return 1
	local version=$1
	local major
	local minor
	local micro
	
	major=$(echo "$version" | sed -r "s/([0-9]+)\.[0-9]+.*/\1/")	
	[ "$major" = "$version" ] && return 1

	minor=$(echo "$version" | sed -r "s/[0-9]+\.([0-9]+).*/\1/")	
	[ "$minor" = "$version" ] && return 1
	
	micro=$(echo "$version" | sed -r "s/[0-9]+\.[0-9]+\.([0-9]+).*/\1/")
	[ "$micro" = "$version" ] && micro=0

	outputArray[0]=$major
	outputArray[1]=$minor
	outputArray[2]=$micro
	
	return 0
}

isVersionGreater() {
	[ $# != 2 ] && return 1
	local ver1=$1
	local ver2=$2

	getMajorMinorMicro "$ver1" || return 1
	ver1=("${outputArray[@]}") 

	getMajorMinorMicro "$ver2"
	ver2=("${outputArray[@]}")

	i=0
	while [ "$i" -le 2 ]; do
		[ "${ver1[$i]}" -gt "${ver2[$i]}" ] && return 0
		[ "${ver1[$i]}" -lt "${ver2[$i]}" ] && return 1
	i=$((i+1))	
	done
	return 0
}

