include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
TARGET = $$QTC_LIB_NAME

include(../liger.pri)

# use precompiled header for libraries by default
CONFIG += precompile_header
DEFINES += USING_PCH
isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$PWD/shared/liger_pch.h

win32 {
    DLLDESTDIR = $$LIGER_APP_PATH
}

DESTDIR = $$LIGER_LIBRARY_PATH

osx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/Frameworks/
    QMAKE_LFLAGS += -compatibility_version $$LIGER_COMPAT_VERSION
}
include(rpath.pri)

TARGET = $$qtLibraryTargetName($$TARGET)

TEMPLATE = lib
CONFIG += shared dll
CONFIG -= qt

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

win32 {
    dlltarget.path = $$LIGER_BUILD_TREE/bin
    INSTALLS += dlltarget
} else {
    target.path = $$LIGER_BUILD_TREE/$$LIGER_LIBRARY_BASENAME/liger
    INSTALLS += target
}
