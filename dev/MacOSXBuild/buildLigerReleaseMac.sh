#!/bin/sh
EXPECTED_ARGS=3

if [ $# -ne $EXPECTED_ARGS ]
then
  echo "Usage: buildLigerReleaseMac.sh PATH_TO_QT_BIN PATH_TO_BUILD PATH_TO_SRC"
  exit 0
fi

QTPATH="$1"
LIGERBUILD="$2"
LIGERSRC="$3"

echo QTPATH=$QTPATH
echo LIGERBUILD=$LIGERBUILD
echo LIGERSRC=$LIGERSRC

rm -rf $LIGERBUILD
mkdir $LIGERBUILD
cd $LIGERBUILD

$QTPATH/qmake $LIGERSRC/liger.pro -r -spec macx-clang CONFIG+=x86_64
make -j 20

$QTPATH/macdeployqt $LIGERBUILD/bin/Liger.app
