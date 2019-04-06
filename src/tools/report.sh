#!/bin/sh


RESULT=/tmp/voxin.report

{
    set -x
    uname -a
    find . -maxdepth 2 -name voxin.log -exec cat {} \; 
    cat /etc/os-release
	cat /etc/lsb-release
	DPKG=$(which dpkg)
	if [ -n "$DPKG" ]; then
		"DPKG" -l speech-dispatcher
		"DPKG" -l "*voxin*"
	else
		DNF=$(which dnf || which yum)
		$DNF list installed speech-dispatcher
		$DNF list installed "*voxin*"
	fi
    set +x
} 1>$RESULT 2>&1 

echo "Please email to contact@oralux.org the report file: $RESULT"

