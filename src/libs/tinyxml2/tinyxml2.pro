include(../../ligerlibrary.pri)

dll {
    DEFINES += TINYXML2_EXPORT
}

CONFIG -= qt

HEADERS += \
    tinyxml2.h

SOURCES += \
    tinyxml2.cpp
