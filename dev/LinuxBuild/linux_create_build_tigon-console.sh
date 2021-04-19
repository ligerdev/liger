#!/bin/bash
# Arguments: Build_Directory Destination_Directory Source_Directory Boost_Lib Matlab_Root_Path

EXPECTED_ARGS=5

if [ $# -lt 4 ] || [ $# -gt 5 ]
then
  echo "Usage: linux_create_build.sh Build_Directory Destination_Directory Source_Directory Boost_Library Matlab_Root_Path"
  exit 0
fi

if [ $# -eq 5 ]
then
  MATLAB_ROOT_PATH="$5"
fi

BUILD_DIRECTORY="$1"
DEST_DIRECTORY="$2"
BIN_DIRECTORY=${BUILD_DIRECTORY}/bin
LIGER_BINARY=${BUILD_DIRECTORY}/bin/tigon-console
SOURCE_DIR="$3"
BOOST_LIB="$4"
TMPDIR_NAME="Tigon"


echo Creating temp directory Tigon bin...
TMPDIR=${DEST_DIRECTORY}/${TMPDIR_NAME}
rm -rf $TMPDIR
mkdir  $TMPDIR

echo Copying binaries...
cp -rf ${BUILD_DIRECTORY}/bin ${TMPDIR}/bin
echo Copying libraries...
cp -rf ${BUILD_DIRECTORY}/lib ${TMPDIR}/lib
echo Copying qt.conf...
cp -f ${SOURCE_DIR}/dev/LinuxBuild/qt.conf ${TMPDIR}/bin/qt.conf

## TODO
# Check if QT_DIR is manually installed by the user, or is in the system directory

## TODO
# Check if BOOST_LIB corresponds to a lib directory installed by the user, or is in the system library

# Copy libraries
#echo Copying Qt libraries to Liger/bin...
#if [ -f "${QT_DIR}/lib/libQt5WebEngine.so" ]; then
#  cp -f ${QT_DIR}/lib/lib*.so.* ${TMPDIR}/lib/
#  cp -f ${QT_DIR}/lib/lib*.so ${TMPDIR}/lib/
#else
  # Uncomment if all the Qt5 libraries are required
  #cp -f ${BOOST_LIB}/libQt5*.so ${TMPDIR}/lib/
  #cp -f ${BOOST_LIB}/libQt5*.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5PrintSupport.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5PrintSupport.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5Help.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5Help.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5Sql.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5Sql.so.* ${TMPDIR}/lib/
  
#  cp -f ${BOOST_LIB}/libQt5CLucene.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5CLucene.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5Xml.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5Xml.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5WebEngine.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5WebEngine.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5WebEngineWidgets.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5WebEngineWidgets.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5WebEngineCore.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5WebEngineCore.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5Quick.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5Quick.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5WebChannel.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5WebChannel.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libQt5Qml.so ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libQt5Qml.so.* ${TMPDIR}/lib/

#  cp -f ${BOOST_LIB}/libprotobuf.so.* ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libre2.so.* ${TMPDIR}/lib/
#  cp -f ${BOOST_LIB}/libminizip.so.* ${TMPDIR}/lib/
#  cp -f /usr/lib/libsrtp.so.* ${TMPDIR}/lib/
#fi
# Copy plugins
#echo Copying Qt plugins to Liger/bin/plugins...
#if [ -d "${QT_DIR}/plugins" ]; then
#  cp -rf ${QT_DIR}/plugins ${TMPDIR}/bin/plugins
#else
#  cp -rf ${BOOST_LIB}/qt5/plugins ${TMPDIR}/bin/plugins
#fi

# Copy QtWebEngine
#echo Copying QtWebEngine to Liger/bin...
#if [ -d "${QT_DIR}/libexec" ]; then
#  cp -f ${QT_DIR}/libexec/QtWebEngineProcess ${TMPDIR}/bin/
#else
#  cp -f ${BOOST_LIB}/qt5/libexec/QtWebEngineProcess ${TMPDIR}/bin/
#fi

# Copy QtWebEngine resources
#echo Copying QtWebEngine resources to Liger/bin/resources
#mkdir ${TMPDIR}/bin/resources
#if [ -d "${QT_DIR}/libexec" ]; then
#  cp -f ${QT_DIR}/resources/* ${TMPDIR}/bin/resources/
#else
#  cp -f /usr/share/qt5/resources/* ${TMPDIR}/bin/resources/
#fi

#echo Copying QtWebEngine QML to Liger/qml/QtWebEngine
#if [ -d "${QT_DIR}/qml/QtWebEngine" ]; then
#  mkdir ${TMPDIR}/qml
#  mkdir ${TMPDIR}/qml/QtWebEngine
#  cp -f ${QT_DIR}/qml/QtWebEngine/* ${TMPDIR}/qml/QtWebEngine/
#fi

# Copy QtWebEngine Locales
#echo Copying Qt translations to Liger/bin/translations
#mkdir ${TMPDIR}/bin/translations
#if [ -d "${QT_DIR}/translations" ]; then
#  cp -a ${QT_DIR}/translations/* ${TMPDIR}/bin/translations
#else
#  cp -a /usr/share/qt5/translations/* ${TMPDIR}/bin/translations
#fi

# Copy Boost library
echo Copying Boost library ...
#cp -f ${BOOST_LIB}/libboost*.so.* ${TMPDIR}/bin/
#cp -f ${BOOST_LIB}/libboost*.so ${TMPDIR}/bin/

cp -f ${BOOST_LIB}/libboost_python*.so.* ${TMPDIR}/bin/
cp -f ${BOOST_LIB}/libboost_python*.so ${TMPDIR}/bin/

cp -f ${BOOST_LIB}/libboost_filesystem*.so.* ${TMPDIR}/bin/
cp -f ${BOOST_LIB}/libboost_filesystem*.so ${TMPDIR}/bin

cp -f ${BOOST_LIB}/libboost_system*.so.* ${TMPDIR}/bin/
cp -f ${BOOST_LIB}/libboost_system*.so ${TMPDIR}/bin/


# Copy Matlab engine libraries
if [ -d "${MATLAB_ROOT_PATH}" ]; then
  echo Copying Matlab engine libraries ...
  cp ${MATLAB_ROOT_PATH}/extern/bin/glnxa64/libMatlabDataArray.so ${TMPDIR}/bin/
  cp ${MATLAB_ROOT_PATH}/extern/bin/glnxa64/libMatlabEngine.so ${TMPDIR}/bin/
fi

files=""
## use ldd to get shared libs list ###
files="$(ldd $LIGER_BINARY |  awk '{ print $3 }' | sed -n '/.so.[0-9]/p')"

for i in $files
do
  echo Copying file ${i}
  cp -f $i ${TMPDIR}/bin
done

# create self extracting files
echo "#!/bin/sh -e" > tigon_install.sh
echo "sed -e '1,/^exit$/d' "\$0" | tar xzfv - " >> tigon_install.sh
echo "exit" >> tigon_install.sh

ARCHIVE_NAME=$TMPDIR_NAME
tar czfv - $ARCHIVE_NAME >> tigon_install.sh
chmod +x tigon_install.sh

rm -rf $TMPDIR

