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

