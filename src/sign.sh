#!/bin/bash -evx

. ./conf.inc

for i in amd64 i386; do
    if [ -e "build.$i" ]; then
	rm -rf build.$i.sig
	cp -a build.$i build.$i.sig
	cd build.$i.sig
	dpkg-sig --verbose -k $SIGNING_KEY --sign builder --sign-changes full *.changes
	cd ..
    fi
done
