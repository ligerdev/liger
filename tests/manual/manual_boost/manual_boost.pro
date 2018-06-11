include(../manual.pri)
CONFIG -= qt
SOURCES += \
    manual_boost.cpp

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
