include(../../ligerlibrary.pri)

dll {
    DEFINES += LIGER_JSON_LIB
} else {
    DEFINES += LIGER_JSON_STATIC_LIB
}

CONFIG -= qt
CONFIG += warn_off

HEADERS += \
    json.h \
    json_global.h

SOURCES += \
    json.cpp
