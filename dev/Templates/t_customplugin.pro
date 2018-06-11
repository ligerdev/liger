# After adding classes that inherit QObject and have Q_OBJECT functionality,
# for example signals or slots, you have to "force" qtcreator to reparse the
# subproject (e.g. make a null change to the *.pro file) so that the makefiles
# are regenerated and new MOC files are created for the new or modified classes.

CONFIG  += exceptions
QT      +=
DEFINES += {% filter upper %}{{ plugin_name }}{% endfilter %}_LIBRARY

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
SOURCES += {% filter lower %}{{ plugin_name }}{% endfilter %}.cpp

HEADERS += {% filter lower %}{{ plugin_name }}{% endfilter %}.h \
           {%+ filter lower %}{{ plugin_name }}{% endfilter %}_global.h \
           {%+ filter lower %}{{ plugin_name }}{% endfilter %}constants.h

OTHER_FILES = {{ plugin_name }}.pluginspec.in

RESOURCES +=

FORMS +=

