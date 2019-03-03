
askContinue() {
    local status
	_gettext "Your version of speech-dispatcher is too old (required version >= $SPEECHD_MIN_VERSION)"
	_gettext "If the installation continues, orca will not be able to use voxin, but emacspeak will"
	_gettext "Do you really want to continue?"
	_gettext "If yes, press the ENTER key."
	yes
	status=$?
    return $status
}

askInstallLang()
{
    local status
	local list=$(getViavoiceTarballs)
    if [ -z "list" ]; then
		status=1
    elif [ ! -e "/opt/IBM/ibmtts" ]; then
		status=0
    else
		_gettext "Do you want to install a new language?"
		_gettext "If yes, press the ENTER key."
		yes
		status=$?
    fi
    return $status
}

askUpdateConfAuthorization() {
    local status
	_gettext "Voxin can be set as default text-to-speech."
	_gettext "Do you authorize the update of the configuration files (speechd.conf)?"
	_gettext "If yes, press the ENTER key."
	yes
	status=$?
    return $status
}

askUninstall()
{
    _gettext "Do you want to uninstall the TTS?"
    _gettext "If you agree please type OK ";
    ok
    return $?
}

askLicense()
{
	local VIAVOICE_ARCHIVE=$(getViavoiceAllTarball)
	local VIAVOICE_AGREEMENT=opt/oralux/voxin/share/doc/voxin-viavoice-all/AGREEMENT
	local VIAVOICE_LICENSE=opt/IBM/ibmtts/doc/IBM.txt
	local RFS_ARCHIVE=$(getRootfilesystemTarball)
	local LIBVOXIN_LGPL=opt/oralux/voxin/share/doc/libvoxin/LGPL.txt
	local LESS
	local CLEAR
	
    if [ "$TERM" = "dumb" ]; then
		LESS=cat
		CLEAR=
    else
		LESS="less -e"
		CLEAR=clear
    fi

	_gettext "Do you agree the licenses?"
    _gettext "If yes, press the ENTER key."    
    yes && return 0

    _gettext "1. Do you want to read again the End-User Agreement?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		if [ "$TERM" != "dumb" ]; then
			_gettext "Please use the space bar to scroll through the document."
		fi
		
		case "$LANG" in
			fr*) VIAVOICE_AGREEMENT="${VIAVOICE_AGREEMENT}-fr";;
			*) ;;
		esac
		
		$CLEAR
		tar -Oxf "$VIAVOICE_ARCHIVE" "$VIAVOICE_AGREEMENT" | $LESS 
    }

    $CLEAR
    _gettext "If you agree to the End-User Agreement, press the ENTER key"
    yes || return 1

    $CLEAR
    _gettext "2. Do you want to read again the IBM License Agreement?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		$CLEAR
		tar -Oxf "$VIAVOICE_ARCHIVE" "$VIAVOICE_LICENSE" | $LESS 
    }

    $CLEAR
    _gettext "If you agree to the IBM license agreement, press the ENTER key"
    yes || return 1

    $CLEAR
    _gettext "3. Do you want to read again the LGPL License?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		$CLEAR
		tar -Oxf "$RFS_ARCHIVE" "$LIBVOXIN_LGPL" | $LESS 
    }	

    $CLEAR
    _gettext "If you agree to the LGPL license, press the ENTER key"
    yes || return 1
    
    $CLEAR
    return 0
}

