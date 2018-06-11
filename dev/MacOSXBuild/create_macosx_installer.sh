#!/bin/bash

EXPECTED_ARGS=3

if [ $# -ne $EXPECTED_ARGS ]
then
  echo "Usage: create_macosx_installer.sh PATH_TO_BINARY BINARY_NAME PATH_TO_QT_BIN"
  exit 0
fi

# Binary.
CDIR=$(pwd)
echo $CDIR
"$3"/macdeployqt "$1"/"$2.app" -dmg
mv "$1"/"$2.dmg" $CDIR