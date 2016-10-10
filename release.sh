#!/bin/bash -evx

. ./conf.inc

PV=$MAJ.$MIN.$REV

#apt-get install debhelper

function build_pkg() {
    local PN=$1
    local ARCH=
    if [ -n "$2" ]; then
	ARCH="-a $2"
    fi
    local BUILD_DIR=build/$PN-$PV
    rm -rf $BUILD_DIR
    mkdir -p $BUILD_DIR

    cp build/$ARCHIVE $BUILD_DIR/${PN}_$PV.orig.tar.gz
    tar -C $BUILD_DIR -zxf build/$ARCHIVE
    mv $BUILD_DIR/libvoxin-$PV $BUILD_DIR/$PN-$PV
    
    cp -a debian.$PN $BUILD_DIR/$PN-$PV/debian
    pushd $BUILD_DIR/$PN-$PV
    dpkg-buildpackage -uc -us $ARCH
    popd
    rm -rf $BUILD_DIR/$PN-$PV
    find build -name "*deb" | while read i; do echo "--> $i"; lintian -i $i; done
}

[ ! -d build ] && mkdir build
cd build
download
cd ..

if [ "$(uname -m)" = "x86_64" ]; then
    rm -rf build.amd64
    mkdir -p build.amd64
    
    build_pkg libvoxin1
    mv build/libvoxin1-$PV/* build.amd64
else
    rm -rf build.i386
    mkdir -p build.i386    

    build_pkg voxind i386
    mv build/voxind-$PV/* build.i386

    build_pkg libvoxin1 i386
    mv build/libvoxin1-$PV/* build.i386
fi

