DEFINES += EXTENSIONSYSTEM_LIBRARY
include(../../ligerlibrary.pri)

CONFIG += qt

unix:LIBS += $$QMAKE_LIBS_DYNLOAD

!isEmpty(vcproj) {
    DEFINES += LIGER_TEST_DIR=\"$$LIGER_SOURCE_TREE\"
} else {
    DEFINES += LIGER_TEST_DIR=\\\"$$LIGER_SOURCE_TREE\\\"
}

win32-msvc* {
    QMAKE_CXXFLAGS += -wd4267
}

HEADERS += pluginerrorview.h \
    plugindetailsview.h \
    invoker.h \
    iplugin.h \
    iplugin_p.h \
    extensionsystem_global.h \
    pluginmanager.h \
    pluginmanager_p.h \
    pluginspec.h \
    pluginspec_p.h \
    pluginview.h \
    optionsparser.h \
    plugincollection.h \
    pluginerroroverview.h
SOURCES += pluginerrorview.cpp \
    plugindetailsview.cpp \
    invoker.cpp \
    iplugin.cpp \
    pluginmanager.cpp \
    pluginspec.cpp \
    pluginview.cpp \
    optionsparser.cpp \
    plugincollection.cpp \
    pluginerroroverview.cpp
FORMS += \
    pluginerrorview.ui \
    plugindetailsview.ui \
    pluginerroroverview.ui
RESOURCES += pluginview.qrc
