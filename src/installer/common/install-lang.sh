#!/bin/bash

### rte ###

Control=./rte/control
Data=./rte/data

if [ -e /opt/IBM/ibmtts/bin/inifilter ]; then
    "$Control"/prerm
fi

"$Control"/preinst

# inst
tar -C "$Data" -cf - . | tar -C / --no-overwrite-dir -xf -

#install=1 (update=1)
"$Control"/postinst 1

### lang ###

Control=./lang/control
Data=./lang/data

#"$Control"/preinst

# inst
tar -C "$Data" -cf - . | tar -C / --no-overwrite-dir -xf -

#install=1 (update=1)
"$Control"/postinst 1

ldconfig /usr/lib
