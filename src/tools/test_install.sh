#!/bin/bash

#set -x

FILE=voxin-american-english-allison-compact-3.4.tgz

cat /proc/"$PPID"/cmdline | grep -q script || { echo "Error, run this test using: script -qc ./test_install.sh"; exit 1; }

[ -f "$FILE" ] || { echo "Archive $FILE not found. Please check that this install script is located in the same directory as $FILE."; exit 1; }

[ "$(id -u)" != 0 ] || { echo "Error, run this script as a regular user (not as super user)"; exit 1; }

echo "This script will install the archive: $FILE"
echo "Once this script is finished, send back the file named typescript to contact@oralux.org for review"
echo
while [ 1 ]; do
    read -p "to start please type y and then ENTER " a
    case "$a" in
	y|Y) break;;
	*) echo "";;
    esac
done

echo "check the md5sum of the archive:"
md5sum voxin-american-english-allison-compact-3.4.tgz

echo "check the filesystem:"
df -hT .

echo "uncompress the archive:"
tar -xf voxin-american-english-allison-compact-3.4.tgz
cd voxin-3.4
cd voxin-american-english-allison-compact-3.4

echo "launch the voxin installer:"
sudo --login $PWD/voxin-installer.sh

echo

echo "Please send back the file named typescript to contact@oralux.org for review"


