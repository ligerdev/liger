# NOTE: If you are trying to debug (i.e. step through) the tests, make sure you disable
# QML in the Debugger settings. Namely, go to Projects locate Debugger Settings and
# uncheck "Enable QML".

# Make sure the dependencies are included before qttest.pri, otherwise they are not parsed.
QTC_LIB_DEPENDS += tigon
include(../../auto.pri)

macx {

}
win32 {
#QMAKE_CXXFLAGS = /O2 /F1000000000000000
}


SOURCES += \
    tst_sparego.cpp
