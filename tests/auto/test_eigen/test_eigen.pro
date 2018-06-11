# NOTE: If you are trying to debug (i.e. step through) the tests, make sure you disable
# QML in the Debugger settings. Namely, go to Projects locate Debugger Settings and
# uncheck "Enable QML".

include(../auto.pri)

# Make sure you enable compiler optimization, otherwise Eigen becomes really slow
# when operating on dynamically allocated matrices!
macx {
  QMAKE_CXXFLAGS += -O2
}
#QMAKE_CXXFLAGS += -O3
win32 {
  QMAKE_CXXFLAGS = /O2
}

SOURCES += \
    tst_eigen.cpp

