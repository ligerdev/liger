# Make sure the dependencies are included before qttest.pri, otherwise they are not parsed.
QTC_LIB_DEPENDS += tigon
include(../../manual.pri)

SOURCES += \
    manual_moga.cpp

