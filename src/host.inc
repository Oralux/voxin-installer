# Settings of each build server (hostname, username)
# BUILD_HOST: host address
# BUILD_USER: user id @ this host

getBuildServer() {
    local arch=$1
    case "$arch" in
	x86_64)
	    BUILD_HOST=debian8-x86-64.local # Debian 8, x86_64
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
	*)
	    echo "unsupported arch"
	    exit 1
	    ;;
    esac
    
    # install directories
    VMVOXDIR=/home/"$BUILD_USER"/VOXIN
    VMLIBVOXDIR="$VMVOXDIR"/LIBVOXIN
}

