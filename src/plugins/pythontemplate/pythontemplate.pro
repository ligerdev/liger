# After adding classes that inherit QObject and have Q_OBJECT functionality,
# for example signals or slots, you have to "force" qtcreator to reparse the
# subproject (e.g. make a null change to the *.pro file) so that the makefiles
# are regenerated and new MOC files are created for the new or modified classes.

CONFIG  += exceptions
DEFINES += PYTHONTEMPLATE_LIBRARY

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport widgets

include(../../ligerplugin.pri)

linux-* {
    # Linux specific commands here.
}

macx {
    # Mac specific commands here.
}

win32 {
    # Windows 32-bit specific commands here.
}

# Source files
SOURCES += pythontemplate.cpp \
           pythontemplatewizard.cpp \
           pythontemplatedialog.cpp \
           pythontempaltewizardpage.cpp

HEADERS += pythontemplate.h \
           pythontemplatewizard.h \
           pythontemplatedialog.h \
           pythontempaltewizardpage.h \
    pythontemplate_global.h \
    pythontemplateconstants.h

OTHER_FILES = PythonTemplate.pluginspec.in

RESOURCES += \
    template.qrc

FORMS += \
    pythontempaltewizardpage.ui

