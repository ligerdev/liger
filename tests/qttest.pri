include(../liger.pri)
include(../src/libs/boost.pri)
include(qttestrpath.pri)

isEmpty(TEMPLATE):TEMPLATE=app
CONFIG -= app_bundle
CONFIG += warn_on console depend_includepath
CONFIG += qt testcase
QT = core

DEFINES -= QT_NO_CAST_FROM_ASCII
DESTDIR = $${TEST_BIN_PATH}

win32 {
    lib = $$LIGER_LIBRARY_PATH;$$LIGER_PLUGIN_PATH;$$TIGON_PLUGIN_PATH
    lib ~= s,/,\\,g
    # the below gets added to later by testcase.prf
    check.commands += cd . & set PATH=$$lib;%PATH%& cmd /c
}

# helper functions
defineTest(copyToDir) {
    files = $$1
    DDIR = $$2
    win32:DDIR ~= s,/,\\,g

    if(isEmpty(DDIR)) {
        win32 {
        } else {
            DDIR = ./
        }
    }

    for(FILE, files) {
        SOURCEPATH = $$absolute_path($$FILE)
        # Replace slashes in paths with backslashes for Windows
        win32{
            SOURCEPATH ~= s,/,\\,g
            DESTPATH ~= s,/,\\,g
        }

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$SOURCEPATH) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}
