include(../../ligerlibrary.pri)

dll {
    DEFINES += LIBAVOID_EXPORTS
}

INCLUDEPATH += $$PWD
CONFIG -= qt
CONFIG += exceptions

win32 {
    win32-msvc* {
        QMAKE_CXXFLAGS_EXCEPTIONS_ON = -EHs
        DEFINES += NOMINMAX
        #disble warning about conversion from '__int64' to 'unsigned int'
        # and "needs to have dll-interface to be used by clients of class"
        # and performance warning
        QMAKE_CXXFLAGS += -wd4244 -wd4267 -wd4251 -wd4800
    }
}

HEADERS += \
    actioninfo.h \
    assertions.h \
    connectionpin.h \
    connector.h \
    connend.h \
    debug.h \
    debughandler.h \
    dllexport.h \
    geometry.h \
    geomtypes.h \
    graph.h \
    hyperedge.h \
    hyperedgeimprover.h \
    hyperedgetree.h \
    junction.h \
    libavoid.h \
    makepath.h \
    mtst.h \
    obstacle.h \
    orthogonal.h \
    router.h \
    scanline.h \
    shape.h \
    timer.h \
    vertices.h \
    viscluster.h \
    visibility.h \
    vpsc.h

SOURCES += \
    actioninfo.cpp \
    connectionpin.cpp \
    connector.cpp \
    connend.cpp \
    geometry.cpp \
    geomtypes.cpp \
    graph.cpp \
    hyperedge.cpp \
    hyperedgeimprover.cpp \
    hyperedgetree.cpp \
    junction.cpp \
    makepath.cpp \
    mtst.cpp \
    obstacle.cpp \
    orthogonal.cpp \
    router.cpp \
    scanline.cpp \
    shape.cpp \
    timer.cpp \
    vertices.cpp \
    viscluster.cpp \
    visibility.cpp \
    vpsc.cpp
