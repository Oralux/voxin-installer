#!/bin/bash
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

askInstallLang()
{
    local status
    if [ ! -d "mnt" ]; then
	status=1
    elif [ ! -e "/opt/IBM/ibmtts" ]; then
	status=0
    else
	echo; gettext "Do you want to install a new language?"
	echo; gettext "If yes, press the ENTER key."
	yes
	status=$?
    fi
    return $status
}

askInstallSpeechDriver()
{
    # select a speech driver
    #    echo; gettext "Please, select below the softwares you are interested in:"


    # Emacspeak package not yet available.
    # #     soft="Emacspeak"
    # #     echo; gettext "Do you want to use the TTS with: \$soft."
    # #     echo; gettext "If yes, press the ENTER key."

    # #     yes
    # #     [ "$?" = "1" ] && outloud_build

    local soft="Orca via Speech-Dispatcher"
    echo; gettext "Do you want to use the TTS with:"
    echo -n " $soft."
    echo; gettext "If yes, press the ENTER key."
    yes
    return $?
}

askUninstall()
{
    echo; gettext "Do you want to uninstall the TTS?"
    echo; gettext "If you agree please type OK ";
    ok
    return $?
}

askLicense()
{
    echo; gettext "Do you agree the licenses?"
    echo; gettext "If yes, press the ENTER key."    
    yes && return 0

    echo; gettext "1. Do you want to read again the End-User Agreement?"
    echo; gettext "If yes, press the ENTER key."    
    yes && {
	if [ "$TERM" != "dumb" ]; then
	    echo; gettext "Please use the space bar to scroll through the document."
	fi
	
	case "$LANG" in
	    fr*)
		AGREEMENT="LICENSE/AGREEMENT-fr"
		;;
	    *)
		AGREEMENT="LICENSE/AGREEMENT"
		;;
	esac
	
	$CLEAR
	iconv -f ISO8859-1 $AGREEMENT | $LESS 
    }

    $CLEAR
    echo; gettext "If you agree to the End-User Agreement, press the ENTER key"
    yes || return 1

    $CLEAR
    echo; gettext "2. Do you want to read again the IBM License Agreement?"
    echo; gettext "If yes, press the ENTER key."    
    yes && {
	$CLEAR
	$LESS LICENSE/IBM.txt
    }

    $CLEAR
    echo; gettext "If you agree to the IBM license agreement, press the ENTER key"
    yes || return 1

    $CLEAR
    echo; gettext "3. Do you want to read again the LGPL License?"
    echo; gettext "If yes, press the ENTER key."    
    yes && {
	$CLEAR
	$LESS LICENSE/LGPL.txt
    }	

    $CLEAR
    echo; gettext "If you agree to the LGPL license, press the ENTER key"
    yes || return 1
    
    $CLEAR
    return 0
}

