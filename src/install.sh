#!/bin/bash -xe

BASE=$(realpath $(dirname "$0")/..)
. $BASE/src/conf.inc

declare -a NAMES=(voxin-rfs32 voxin-libstdc++ voxind libvoxin1 libvoxin1-dev)
declare -a ARCHS=(all         all             all    amd64     all)
MAX_NAME=${#NAMES[@]}
i=0
while [ "$i" -lt "$MAX_NAME" ]; do
	NAME=${NAMES[$i]}
	ARCH=${ARCHS[$i]}
	sudo dpkg -i ${PKGDIR}/${NAME}*${ARCH}.deb || true
	i=$((i+1))
done





