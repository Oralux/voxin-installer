#!/bin/bash
# This file is under the LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#

askInstallLang()
{
    return $((! with_lang))
}

askInstallSpeechDriver()
{
    return $((! with_sd))
}

askUninstall()
{
    return $((! with_uninstall))
}

askLicense()
{
    return 0
}

