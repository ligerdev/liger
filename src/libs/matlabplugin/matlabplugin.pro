include(matlab.pri)
include(../boost.pri)
include(../../tigonplugin.pri)

CONFIG += warn_off

dll {
    DEFINES += LIGER_TIGON_LIB
} else {
    DEFINES += LIGER_TIGON_STATIC_LIB
}

HEADERS += MatlabPlugin.h
SOURCES += MatlabPlugin.cpp

# Include matlab support
!isEmpty(MATLAB_ROOT_PATH) {
    HEADERS += \
        Representation/Functions/Matlab/MatlabFunction.h \
        Utils/MatlabPool.h \
        Utils/MatlabEngine.h

    SOURCES += \
        Representation/Functions/Matlab/MatlabFunction.cpp \
        Utils/MatlabPool.cpp \
        Utils/MatlabEngine.cpp
}

# Boost
win32: LIBS += -L$$(BOOST_LIB)
macx {
    LIBS += $$(BOOST_LIB)/libboost_filesystem-mt.dylib \
            $$(BOOST_LIB)/libboost_system-mt.dylib
}
linux-* {
    LIBS += $$(BOOST_LIB)/libboost_filesystem.so \
            $$(BOOST_LIB)/libboost_system.so
}



