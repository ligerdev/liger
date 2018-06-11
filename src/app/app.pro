include(../../liger.pri)
include(../shared/qtsingleapplication/qtsingleapplication.pri)

TEMPLATE = app
TARGET = $$LIGER_APP_TARGET
DESTDIR = $$LIGER_APP_PATH

SOURCES += main.cpp

include(../rpath.pri)

LIBS *= -l$$qtLibraryName(ExtensionSystem) -l$$qtLibraryName(Aggregation) -l$$qtLibraryName(Utils)

QT_BREAKPAD_ROOT_PATH = $$(QT_BREAKPAD_ROOT_PATH)
!isEmpty(QT_BREAKPAD_ROOT_PATH) {
    include($$QT_BREAKPAD_ROOT_PATH/qtbreakpad.pri)
}
win32 {
    RC_FILE = app.rc
    target.path = $$LIGER_BUILD_TREE/bin
    INSTALLS += target

} else:macx {
    LIBS += -framework CoreFoundation
    ICON = liger.icns
    FILETYPES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += FILETYPES
    info.input = Info.plist.in
    info.output = $$LIGER_BIN_PATH/../Info.plist
    QMAKE_SUBSTITUTES = info

} else {
    target.path  = $$LIGER_BUILD_TREE/bin
    INSTALLS    += target
}

OTHER_FILES += Info.plist.in \
    $$PWD/app_version.h.in

QMAKE_SUBSTITUTES += $$PWD/app_version.h.in

CONFIG += no_batch
