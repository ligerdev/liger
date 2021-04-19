
QTC_LIB_DEPENDS += tigon

include(../matlabcheck.pri)
contains(DEFINES, MATLAB_API) {
    QTC_LIB_DEPENDS += matlabplugin
}

include(../liger.pri)
include(../src/libs/boost.pri)

LIBS -= -L$$LIGER_PLUGIN_PATH

linux-* {

    LIGER_PLUGIN_PATH = $$LIGER_BUILD_TREE/$$LIGER_LIBRARY_BASENAME/tigon/plugins

    QMAKE_RPATHDIR += $$LIGER_BUILD_TREE/$$LIGER_LIBRARY_BASENAME/tigon
    QMAKE_RPATHDIR += $$LIGER_BUILD_TREE/bin
    QMAKE_RPATHDIR += $$LIGER_PLUGIN_PATH

    LIGER_PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${LIGER_PLUGIN_RPATH}\'
} else:macx {
    QMAKE_LFLAGS += -Wl,-rpath,\"$$LIGER_BIN_PATH/../\"
}

LIBS *= -L$$LIGER_PLUGIN_PATH

TEMPLATE = app
CONFIG -= app_bundle
CONFIG += warn_on cmdline depend_includepath
CONFIG -= qt
QT = core

DEFINES -= QT_NO_CAST_FROM_ASCII
DESTDIR = $$LIGER_BUILD_TREE/bin

TARGET = tigon-console

unix {
  QMAKE_CXXFLAGS += -O2
}

SOURCES += console.cpp

