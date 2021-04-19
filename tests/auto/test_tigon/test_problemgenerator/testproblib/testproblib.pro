TEMPLATE = lib

QTC_LIB_DEPENDS += tigon
include(../../../../qttest.pri)

PRECOMPILED_HEADER=

CONFIG  -= app_bundle qt testcase
TARGET  = testprob # overwrite the TARGET name

SOURCES += testprob.cpp
HEADERS += testprob.h
