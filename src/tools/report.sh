#!/bin/sh


RESULT=/tmp/voxin.report

if [ "$1" != "-f" ] && [ "$UID" = 0 ]; then
	echo "Run this script as normal user (not superuser)"
	exit 1
fi

{
    set -x
    uname -a
    find . -maxdepth 2 -name voxin.log -exec cat {} \; 
    cat /etc/os-release
	cat /etc/lsb-release
	DPKG=$(which dpkg)
	if [ -n "$DPKG" ]; then
		"$DPKG" -l speech-dispatcher
		"$DPKG" -l "*voxin*"
	else
		DNF=$(which dnf || which yum)
		$DNF list installed speech-dispatcher
		$DNF list installed "*voxin*"
	fi

	# check speech-dispatcher setup
	spd-say -O
	spd-say -L
	
    CONF=$(ls -t /home/*/.speech-dispatcher/conf/speechd.conf /home/*/.config/speech-dispatcher/speechd.conf /etc/speech-dispatcher/speechd.conf /usr/share/speech-dispatcher/conf/speechd.conf 2>/dev/null)
	for FILE in $CONF; do
		echo "--> $FILE"
		cat $FILE
	done


    echo "### ps"
    ps -ef
	
    set +x
} 1>$RESULT 2>&1 

echo "Email please to contact@oralux.org this report file: $RESULT"

