#!/bin/bash
FILE=/tmp/voxin_report.md5
rm -f $FILE
find /opt/oralux -type f | while read i; do md5sum $i >> $FILE; done
