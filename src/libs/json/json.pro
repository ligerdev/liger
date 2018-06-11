include(../../ligerlibrary.pri)

dll {
    DEFINES += LIGER_JSON_LIB
} else {
    DEFINES += LIGER_JSON_STATIC_LIB
}

CONFIG -= qt

HEADERS += \
    json.h \
    json_global.h

SOURCES += \
    json.cpp
