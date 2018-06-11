include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
TARGET = $$QTC_LIB_NAME

include(../liger.pri)

# use precompiled header for libraries by default
CONFIG += precompile_header
DEFINES += USING_PCH
isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$PWD/libs/tigon/tigon_pch.h

isEmpty(TIGON_PLUGIN_PATH) {
    macx {
        TIGON_PLUGIN_PATH  = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/TigonPlugIns
    } else {
        TIGON_PLUGIN_PATH  = $$LIGER_BUILD_TREE/$$LIGER_LIBRARY_BASENAME/tigon/plugins
    }
}

win32 {
    DLLDESTDIR = $$TIGON_PLUGIN_PATH
}

DESTDIR = $$TIGON_PLUGIN_PATH

osx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/TigonPlugIns/
    QMAKE_LFLAGS += -compatibility_version $$LIGER_COMPAT_VERSION
}
include(rpath.pri)

TARGET = $$qtLibraryTargetName($$TARGET)

TEMPLATE = lib
CONFIG += plugin shared dll
CONFIG -= qt

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

win32 {
    dlltarget.path = $$TIGON_PLUGIN_PATH
    INSTALLS += dlltarget
} else {
    target.path = $$TIGON_PLUGIN_PATH
    INSTALLS += target
}
