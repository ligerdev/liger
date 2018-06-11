#!/bin/bash

current=`pwd`;
bindir=`cd ${current}/../../bin;pwd`
LD_LIBRARY_PATH=$bindir:${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

rm test_*.log
rm test_*.xml
for FILE in `find . -executable -name 'test_*' -type f`
do
    ${FILE} -silent -xunitxml -o ${FILE}.xml > ${FILE}.log &
done

while :
do
#    clear
    output="$(ps -ef | grep test_ | grep -v grep)"
#    ps -ef | head -1
#    echo "${output}"
    if [ -z "${output}" ]; then
        echo "Done"
        break
    fi
    sleep 10
done
