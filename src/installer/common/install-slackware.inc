getVoxinPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/slackware -name "voxin*-$ARCH*"
}

getPackageVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	local version
	
	version=$(find /var/log/packages -name "$package*" | while read a; do
		a=$(basename "$a")
		a=${a%-*}; a=${a%-*}
		[ "${a%-*}" = "$package" ] && echo "res=${a##*-}" && break
	done)

	case $version in
	    res=*) version=${version#res=};;
	    default) unset version;;
	esac	

	echo "$version"	
}

isPackageInstalled() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local package=$1
	[ -n "$(getPackageVersion $package)" ]
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
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	installpkg "$package" &>>"$LOG"
}

installPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	local cmd=$(which slackpkg) || return 1
	slackpkg -batch=on -default_answer=y install "$package" &>>"$LOG"
}

uninstallPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	removepkg"$package" &>>"$LOG"
}

