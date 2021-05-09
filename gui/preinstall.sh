#!/bin/sh

BASE="$(dirname $(realpath "$0"))"
LOG=${1:-/tmp/voxin-installer.log.$$}

date >> $LOG
echo "--> Starting preinstall.sh $$" >> $LOG
sleep 1

cd $BASE
setsid ./preinstall2.sh "$LOG"
echo "--> Stopping preinstall.sh $$" >> $LOG
date >> $LOG
sleep 5

