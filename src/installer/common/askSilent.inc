askContinue() {
    return 1
}

askInstallLang()
{
    return $((! with_lang))
}

askUninstall()
{
    return $((! with_uninstall))
}

askLicense()
{
    return 0
}

