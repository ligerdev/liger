rm test_*.log
for FILE in `find . -name 'test_*' -type f`
do
    echo Start ${FILE}; ${FILE} > ${FILE}.log &
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
for LOG in `find . -name 'test_*.log' -type f`
do
    echo ${LOG}
    cat ${LOG} | tail -2 | head -1
done
