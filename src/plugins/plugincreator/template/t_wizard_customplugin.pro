# After adding classes that inherit QObject and have Q_OBJECT functionality,
# for example signals or slots, you have to "force" qtcreator to reparse the
# subproject (e.g. make a null change to the *.pro file) so that the makefiles
# are regenerated and new MOC files are created for the new or modified classes.

CONFIG  += exceptions
QT      +=
DEFINES += {{ UPPER_CLASS_PLUGIN_NAME }}_LIBRARY

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
SOURCES += {{ LOWER_CLASS_PLUGIN_NAME }}.cpp

HEADERS += {{ LOWER_CLASS_PLUGIN_NAME }}.h \
           {{ LOWER_CLASS_PLUGIN_NAME }}_global.h \
           {{ LOWER_CLASS_PLUGIN_NAME }}constants.h

OTHER_FILES = {{ plugin_name }}.pluginspec.in

RESOURCES +=

FORMS +=

