# This project file generates a command line application for Tigon without Qt.
# It is intended to run Tigon from a command line interface, and work only on unix for the moment.
# Please use liger.pro for compiling the whole Liger project.

include(liger.pri)

TEMPLATE = subdirs
CONFIG  += silent
CONFIG -= qt
CONFIG += ordered

SUBDIRS = src/libs/botan \
    src/libs/eigen \
    src/libs/fann  \
    src/libs/json  \
    src/libs/tinyxml2 \
    src/libs/tigon

# Include matlab plugin
include(matlabcheck.pri)
contains(DEFINES, MATLAB_API) {
  message("Include MATLAB plugin")
  SUBDIRS += src/libs/matlabplugin
} else {
  message("Does NOT include MATLAB plugin")
}

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$$basename(l)_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}

unix: SUBDIRS += console

TMP = $$(BOOST_LIB)
linux-* {
    deploy.commands = $$PWD/dev/LinuxBuild/linux_create_build_tigon-console.sh $${OUT_PWD} $${OUT_PWD} $${PWD} $$TMP
    contains(DEFINES, MATLAB_API) {
      deploy.commands = $$PWD/dev/LinuxBuild/linux_create_build_tigon-console.sh $${OUT_PWD} $${OUT_PWD} $${PWD} $$TMP $${MATLAB_ROOT_PATH}
    }
    QMAKE_EXTRA_TARGETS += deploy
}
