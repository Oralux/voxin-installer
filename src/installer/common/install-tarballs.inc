# In case of a regular user install, the (slackware) tarballs are
# uncompressed under the user's target directory ($TOPDIR)
getVoxinPackage() {
    [ -z "$ARCH" ] && return 1
    find packages/slackware -name "voxin-[0-9]*-$ARCH*"
}

getVoxinSpeechdPackage() {
    [ -z "$ARCH" ] && return 1
    find packages/slackware -name "voxin-speechd-[0-9]*$ARCH*"
}

getVoxinSpeechdConfPackage() {
    [ -z "$ARCH" ] && return 1
    find packages/slackware -name "voxin-speechd-conf-[0-9]*$ARCH*"
}

is_sd_voxin_installed() {
    #not applicable
    return 1
}

getPackageVersion() {
    #not applicable
    return 0
}

isPackageInstalled() {
    # not applicable
    return 0
}

# getPackageFileVersion
# Example:
# input: name1-name2-1.2.3-x86_64-1.txz
# output: 1.2
getPackageFileVersion() {
    unset output
    [ $# != 1 ] || [ -z "$1" ] && return
    local package=$1
    local a	
    a=${package%-*}; a=${a%-*}; a=${a##*-}
    output=$(echo $a | cut -f1-2 -d.)	
}

installLocalPackage() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local package=$1
    echo "installLocalPackage $package" >> "$LOG" 2>&1
    [ ! -d "$TOPDIR" ] && return 1
    tar -C "$TOPDIR" -xf "$package" && ( cd "$TOPDIR"; . ./install/doinst.sh; rm -rf install/; )
}

installPackage() {
    # not applicable
    return
}

uninstallPackage() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    # not applicable
    return 0
}

postInstSystem() {
    cp -a "$TOPDIR"/usr/doc/* "$TOPDIR"/usr/share/doc/
    rm -rf "$TOPDIR"/usr/doc
}

installLocalSpeechdModule() {
    local module_orig="$TOPDIR"/opt/oralux/voxin/lib/speech-dispatcher-modules/sd_voxin
    local module="$SPEECHD_MODULE_DIR_USER/sd_voxin"
    if [ ! -a "$module" ]; then
	[ -h "$module" ] && unlink "$module"
	mkdir -p "$SPEECHD_MODULE_DIR_USER"
	ln -s "$module_orig" "$module"
    fi
}

uninstallLocalSpeechdModule() {
    local module="$SPEECHD_MODULE_DIR_USER/sd_voxin"   
    [ -h "$module" ] && unlink "$module"
}
