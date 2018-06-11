#!/bin/sh
# runOneTest.sh test_folder test_executable output_txt
DYLD_LIBRARY_PATH=$1:$DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH

"$2" > "$3"
