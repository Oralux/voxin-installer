#!/bin/bash

RFSDIR=$1
ECI="$RFSDIR"/var/opt/IBM/ibmtts/cfg/eci.ini
mkdir -p $(dirname "$ECI")
cat "$RFSDIR"/opt/IBM/ibmtts/etc/*.ini > "$ECI"
sed -i "s#=/opt/#=$RFSDIR/opt/#" "$ECI"	

