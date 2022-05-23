askContinue() {
    return 1
}

askInstallLang()
{
    return $((! with_lang))
}

askUpdateConfAuthorization() {
    return $((! with_sd))
}

askUninstall()
{
    return $((! with_uninstall))
}

askRemoveDir()
{
    return $((! with_uninstall))
}

askLicense=noOp

