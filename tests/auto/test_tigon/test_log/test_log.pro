# NOTE: If you are trying to debug (i.e. step through) the tests, make sure you disable
# QML in the Debugger settings. Namely, go to Projects locate Debugger Settings and
# uncheck "Enable QML".

# Make sure the dependencies are included before qttest.pri, otherwise they are not parsed.
QTC_LIB_DEPENDS += tigon
include(../../auto.pri)

#QT -= gui
# Make sure you enable compiler optimization, otherwise Eigen becomes really slow
# when operating on dynamically allocated matrices!
# QMAKE_CXXFLAGS += -O3

SOURCES += \
    tst_log.cpp
