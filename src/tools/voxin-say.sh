#!/bin/sh

BASE=$(realpath "$(dirname "$0")/..")

## TODO (conf)

# look for the current installation path (local or system-wide)
LOCAL1=$BASE/rfs
LOCAL2=$BASE/.oralux/rfs
GLOBAL=/

VOXINDIR=/opt/oralux/voxin
unset RFSDIR
for i in "$LOCAL1" "$LOCAL2" "$GLOBAL"; do
	[ -d "$i/$VOXINDIR" ] && RFSDIR=$i && break
done	

##

[ -z "$RFSDIR" ] && echo "voxin directory not found!" && exit 1

unset PLAY
for i in aplay paplay; do
	PLAY=$(which $i 2>/dev/null) && break
done

./setConf.sh "$RFSDIR"

export LD_LIBRARY_PATH="$RFSDIR/$VOXINDIR/lib"

if [ -n "$PLAY" ]; then
	"$RFSDIR/$VOXINDIR"/bin/voxin-say | $PLAY || unset PLAY 
fi

if [ -z "$PLAY" ]; then
	"$RFSDIR/$VOXINDIR"/bin/voxin-say -w /tmp/voxin-say.$$.wav
	echo "/tmp/voxin-say.$$.wav"
fi

#"$VOXINDIR"/bin/voxin-say."$(uname -m)" $@


