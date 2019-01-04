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
	a=$(which $i) && PLAY=$i && break
done

[ -z "$PLAY" ] && echo "install aplay or paplay" && exit 1

./setConf.sh "$RFSDIR"

export LD_LIBRARY_PATH="$RFSDIR/$VOXINDIR/lib"
"$RFSDIR/$VOXINDIR"/bin/voxin-say $@ | $PLAY

#"$VOXINDIR"/bin/voxin-say."$(uname -m)" $@


