#!/bin/sh

BASE="$(dirname $(realpath "$0"))"
LOG=${1:-/tmp/voxin-installer.log.$$}

echo "--> Starting preinstall2.sh $$" >> $LOG

cd $BASE/../lib

if [ ! -e voxin-installer.sh ]; then
  echo "voxin installer not found !" >> $LOG
  sleep 2
  exit 1;
fi

RES=0
COUNT=0
while [ "$RES" = "0" ]; do 
  sleep 0.5; 
  echo "--> sleep preinstall2.sh $$" >> $LOG
  fuser /var/lib/dpkg/lock
  RES=$?
  COUNT=$((COUNT+1))
  [ "$COUNT" -gt 60 ] && { echo "can't lock /var/lib/dpkg/lock" >> $LOG; exit 1; }
done

echo "start voxin-installer.sh" >> $LOG
sleep 2
#set +e
./voxin-installer.sh -l >> $LOG

sleep 2

rm -rf "$VOXIN_DIR"

exit 0
