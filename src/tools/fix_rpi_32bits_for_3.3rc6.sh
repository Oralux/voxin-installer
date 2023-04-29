#!/bin/sh

# Fix for installing voxin 3.3rc6 on Debian ARM 32 bits

sed -i 's/DEBIAN_ARCH=armv7h/DEBIAN_ARCH=armhf/' common/init.inc
