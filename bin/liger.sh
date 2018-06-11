#!/bin/bash

makeAbsolute() {
    case $1 in
        /*)
            # already absolute, return it
            echo "$1"
            ;;
        *)
            # relative, prepend $2 made absolute
            echo `makeAbsolute "$2" "$PWD"`/"$1" | sed 's,/\.$,,'
            ;;
    esac
}

me=`which "$0"` # Search $PATH if necessary
if test -L "$me"; then
    # Try readlink(1)
    readlink=`type readlink 2>/dev/null` || readlink=

    if test -n "$readlink"; then
        # We have readlink(1), so we can use it. Assuming GNU readlink (for -f).
        me=`readlink -nf "$me"`
    else
        # No readlink(1), so let's try ls -l
        me=`ls -l "$me" | sed 's/^.*-> //'`
        base=`dirname "$me"`
        me=`makeAbsolute "$me" "$base"`
    fi
fi

bindir=`dirname "$me"`
libdir=`cd "$bindir/../lib" ; pwd`
qtlibdir=`cd "$bindir" ; pwd`

MATLAB_BIN=`whereis matlab`
if [ -z "$MATLAB_BIN" ]; then  # check if Matlab path exists
  IFS=' ' read -ra MBPATH <<< "$MATLAB_BIN"
  MATLAB_PATH="${MBPATH[1]}"
  MATLAB_PATH=`readlink $MATLAB_PATH`
  MATLAB_PATH=${MATLAB_PATH%"/bin/matlab"}
  MATLAB_INCLUDE_PATH=${MATLAB_PATH}/extern/include
  MATLAB_LIBRARY_PATH=${MATLAB_PATH}/bin/glnxa64
  LD_LIBRARY_PATH=$libdir:$libdir/liger:$qtlibdir:${MATLAB_LIBRARY_PATH}
else
  LD_LIBRARY_PATH=$libdir:$libdir/liger:$qtlibdir
fi

export LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH

QT_PLUGIN_PATH=$qtlibdir/plugins
export QT_PLUGIN_PATH

echo Launched from $me
exec "$bindir/liger" ${1+"$@"}
