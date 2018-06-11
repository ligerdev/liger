include(../../ligerlibrary.pri)

#CONFIG -= shared dll qt
#CONFIG += staticlib
CONFIG -= qt
CONFIG += warn_off

#DEFINES += FANN_NO_DLL
DEFINES += FANN_DLL_EXPORTS

INCLUDEPATH += $$PWD
win32:DEFINES += NOMINMAX

HEADERS += \
    compat_time.h \
    config.h \
    doublefann.h \
    fann.h \
    fann_activation.h \
    fann_cascade.h \
    fann_cpp.h \
    fann_data.h \
    fann_error.h \
    fann_internal.h \
    fann_io.h \
    fann_train.h

SOURCES += \
    doublefann.c \
    fann.c \
    fann_cascade.c \
    fann_error.c \
    fann_io.c \
    fann_train.c \
    fann_train_data.c

