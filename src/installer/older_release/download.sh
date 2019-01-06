#!/bin/bash

VER=0.68
URL=http://voxin.oralux.net/update/voxin-update-$VER.tgz

mkdir $VER
cd $VER
wget $URL
tar -zxf ./voxin-update-$VER.tgz
cp -a $PWD/../../mnt voxin-0.68/voxin-update-0.68
