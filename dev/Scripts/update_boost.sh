LIGER_PATH=~/Projects/Liger
BOOST_PATH=~/Downloads/boost_1_63_0
DEST=${LIGER_PATH}/src/libs/boost-tmp

rm -rf ${DEST}
mkdir ${DEST}

## compile bcp
#${BOOST_PATH}/boostrap
#${BOOST_PATH}/bjam ${BOOST_PATH}/tools/bcp

# Get files using boost
boost_list1=`grep "<boost/" -r ${LIGER_PATH}/src/libs/tigon | awk -F':' '{print $1}' | sort | uniq`
boost_list2=`grep "<boost/" -r ${LIGER_PATH}/tests | awk -F':' '{print $1}' | sort | uniq`

${BOOST_PATH}/dist/bin/bcp --scan --boost=${BOOST_PATH} \
${boost_list1} \
${boost_list2} \
${DEST}

# Clean up
rm -rf ${LIGER_PATH}/src/libs/boost-tmp/libs
rm ${LIGER_PATH}/src/libs/boost-tmp/Jamroot
mv ${DEST}/boost/* ${DEST}
rm -rf ${DEST}/boost