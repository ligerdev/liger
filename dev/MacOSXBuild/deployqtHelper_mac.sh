#!/bin/bash
[ $# -lt 2 ] && echo "Usage: $(basename $0) <app folder> <qt bin>" && exit 2
[ $(uname -s) != "Darwin" ] && echo "Run this script on Mac OS X" && exit 2;

"$2/"macdeployqt "$1" || exit 1