TEMPLATE = lib

CONFIG += plugin
CONFIG += shared dll
QTC_LIB_DEPENDS += tigon
include(../../../../qttest.pri)

PRECOMPILED_HEADER=

CONFIG  -= app_bundle
TARGET  = exampleplugin # overwrite the TARGET name

DEFINES += LIGER_TIGON_LIB

HEADERS +=  exampleplugin.h
SOURCES +=  exampleplugin.cpp
