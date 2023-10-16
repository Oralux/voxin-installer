# Settings of each build server (hostname, username)
# BUILD_HOST: host address
# BUILD_USER: user id @ this host

IP_SLINT=192.168.122.169

getBuildServer() {
    local arch=$1
    case "$arch" in
	x86_64|x86-64)
	    [ -z "$OS" ] && OS=debian10
	    BUILD_HOST="$OS"-x86-64.local # Debian 10, x86_64 (or Arch Linux, Fedora, Slint,...)
	    BUILD_USER=user1	    
	    ;;
	armv7l)
	    BUILD_HOST=rpi-armv7l.local # RaspberryOS armv7l
	    BUILD_USER=user1	    
	    ;;
	aarch64)
	    BUILD_HOST=rpi-aarch64.local # RaspberryOS aarch64
	    BUILD_USER=user1	    
	    ;;
	slint)
	    BUILD_HOST=$IP_SLINT # avahi not yet installed
	    BUILD_USER=user1
	    ;;
	*)
	    echo "unsupported arch"
	    exit 1
	    ;;
    esac
    
    # install directories
    VMVOXDIR=/home/"$BUILD_USER"/VOXIN
    VMLIBVOXDIR="$VMVOXDIR"/LIBVOXIN
}

