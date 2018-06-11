#!/bin/bash

EXPECTED_ARGS=2

if [ $# -ne $EXPECTED_ARGS ]
then
  echo "Usage: copy_shared_libs.sh BINARY DYNAMIC_LIBRARIES_DESITNATION_DIR"
  exit 0
fi

# Binary.
pFILE="$1"
# Destination directory.
d="$2"  
files=""
## use ldd to get shared libs list ###
files="$(ldd $pFILE |  awk '{ print $3 }' | sed  '/^$/d')"

for i in $files
do
  cp -f $i ${d}
done
