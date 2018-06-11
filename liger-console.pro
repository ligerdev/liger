# This is the project file made for generating test executables without Qt.
# It is intended to be used for testing purpose, at least for now.
# Please use liger.pro to compiling the whole Liger project.

include(liger.pri)

TEMPLATE = subdirs
CONFIG  += silent
CONFIG -= qt

SUBDIRS = src/libs/botan \
    src/libs/eigen \
    src/libs/fann  \
    src/libs/json  \
    src/libs/tinyxml2 \
    src/libs/tigon

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$$basename(l)_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}

SUBDIRS += tests/manual/manual_tigon
