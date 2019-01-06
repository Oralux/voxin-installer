#!/bin/sh
# This file is under LGPL license
# March 2007, Gilles Casse <gcasse@oralux.org>
#
# new_po.sh : create or update xx.new.po, (xx = fr,...) the file which 
# includes the translation for the xx language.
#

VOXIN=".."


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

if test -s $lg.new.po
then
  echo "The $lg.new.po file already exists.";
  echo "Do you really want to remove it ? (y/n)";
  read q
  case $q in
    y|Y) ;;
    *) echo "ok, we stop here.";exit 1;;
  esac
fi

touch $lg.def.po $lg.new.po voxin.pot
chmod +w $lg.def.po $lg.new.po voxin.pot

echo "Building the new $lg.new.po..."   

xgettext --add-location -s `find  $VOXIN -maxdepth 2 -name "*sh"` -o voxin.pot
msgmerge $lg.def.po voxin.pot -o $lg.new.po

echo "...the file $lg.new.po is ready"   
