#!/bin/sh

BASE=$(realpath "$(dirname "$0")/..")


## TODO (conf)

# look for the current installation path (local or system-wide)
LOCAL1=$BASE/.oralux/rfs
LOCAL2=$HOME/.oralux/rfs
GLOBAL=/

VOXINDIR=/opt/oralux/voxin
unset RFSDIR
for i in "$LOCAL1" "$LOCAL2" "$GLOBAL"; do
	[ -d "$i/$VOXINDIR" ] && RFSDIR=$i && break
done	

##

./setConf.sh "$RFSDIR"

export LD_LIBRARY_PATH="$RFSDIR/$VOXINDIR/lib"
"$VOXINDIR"/bin/voxin-say $@

#"$VOXINDIR"/bin/voxin-say."$(uname -m)" $@


