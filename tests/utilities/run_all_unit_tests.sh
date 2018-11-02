rm test_*.log
for FILE in `find . -name 'test_*' -type f`
do
    ${FILE} -silent -xunitxml -o ${FILE}.xml > ${FILE}.log &
done

while :
do
    clear
    output="$(ps -ef | grep test_ | grep -v grep)"
    echo "Running unittests: "
    ps -ef | head -1
    echo "${output}"
    if [ -z "${output}" ]; then
        echo "Done"
        break
    fi
    sleep 4
done

echo "Check results"
for RES in `find . -name 'test_*.xml' -type f`
do
    cat ${RES} | head -2 | tail -1 | awk '{ print "<" $5 ": "  $3}'
done
