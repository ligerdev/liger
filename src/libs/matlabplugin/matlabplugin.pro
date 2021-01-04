include(matlab.pri)
include(../boost.pri)
include(../../tigonplugin.pri)

CONFIG += warn_off

dll {
    DEFINES += LIGER_TIGON_LIB
} else {
    DEFINES += LIGER_TIGON_STATIC_LIB
}

HEADERS += MATLABPlugin.h
SOURCES += MATLABPlugin.cpp

# Include matlab support
!isEmpty(MATLAB_ROOT_PATH) {
    HEADERS += \
        Representation/Functions/Matlab/MatlabFunction.h \
        Utils/MatlabPool.h \
        Utils/IMatlabEngine.h

    SOURCES += \
        Representation/Functions/Matlab/MatlabFunction.cpp \
        Utils/MatlabPool.cpp \
        Utils/IMatlabEngine.cpp

    contains(DEFINES, MATLAB_API_CPP) {
       HEADERS += Utils/MatlabEngineX.h
       SOURCES += Utils/MatlabEngineX.cpp
    }

    contains(DEFINES, MATLAB_API_C) {
       HEADERS += Utils/MatlabEngineC.h
       SOURCES += Utils/MatlabEngineC.cpp
    }

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



