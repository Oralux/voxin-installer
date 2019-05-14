#!/bin/bash
DBG=/tmp/voxin.dbg
RESULT=$DBG.tgz

if [ "$UID" = 0 ]; then
	echo "Run this script as normal user (not superuser)"
	exit 1
fi

mkdir $DBG
speech-dispatcher -v > $DBG/version
strace -o $DBG/log -s300 -tt -ff spd-say -o voxin "hello all"
tar -zcf $RESULT $DBG
rm -rf $DBG

echo "Email please to contact@oralux.org this report file: $RESULT"
