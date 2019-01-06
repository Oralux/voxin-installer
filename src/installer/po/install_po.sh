#!/bin/sh
# This file is under LGPL license
# March 2007, Gilles Casse <gcasse@oralux.org>
#
# install_po.sh : install the newly translated messages
#


if [ "$#" != "1" ]; then
    echo "Please, enter your language code (default = $LANG)"
    read lg
else
    lg="$1"
fi

# if lg is empty, $LANG is used 

case v$lg in
  v) lg=$LANG;;
esac

# The directory locale/$lg/LC_MESSAGES is created if it does not exist

LOCALE="$(dirname $(pwd))/locale/$lg/LC_MESSAGES"

install -d $LOCALE

if [ -e "$lg.new.po" ]; then 
    mv $lg.def.po $lg.def.po.bak
    mv $lg.new.po $lg.def.po
fi

msgfmt $lg.def.po -o voxin.mo
mv voxin.mo $LOCALE

echo "...the file voxin.mo is installed in $LOCALE"   
