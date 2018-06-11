!isEmpty(LIGER_PRI_INCLUDED):error("liger.pri already included")
LIGER_PRI_INCLUDED = 1

LIGER_VERSION=1.0.0
LIGER_COMPAT_VERSION=1.0.0

QTCREATOR_VERSION = 4.5.0
QTCREATOR_COMPAT_VERSION = 4.5.0
#VERSION = $$LIGER_VERSION
BINARY_ARTIFACTS_BRANCH = develop

# enable c++11
CONFIG += c++11

# Suppress certain warnings globally
macx: {
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-return-stack-address # clang warning
}
win32: {
  DEFINES += NOMINMAX
}

include(matlabcheck.pri)

defineReplace(cleanPath) {
    return($$clean_path($$1))
}

defineReplace(targetPath) {
    return($$shell_path($$1))
}

defineReplace(qtLibraryTargetName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

defineReplace(qtLibraryName) {
   RET = $$qtLibraryTargetName($$1)
#   win32 {
#      VERSION_LIST = $$split(LIGER_VERSION, .)
#      RET = $$RET$$first(VERSION_LIST)
#   }
   return($$RET)
}

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

# For use in custom compilers which just copy files
defineReplace(stripSrcDir) {
    return($$relative_path($$absolute_path($$1, $$OUT_PWD), $$_PRO_FILE_PWD_))
}

isEmpty(LIGER_LIBRARY_BASENAME) {
    LIGER_LIBRARY_BASENAME = lib
}

LIGER_SOURCE_TREE = $$PWD
isEmpty(LIGER_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    LIGER_BUILD_TREE = $$cleanPath($$OUT_PWD)
    LIGER_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
LIGER_APP_PATH = $$LIGER_BUILD_TREE/bin
macx {
    LIGER_APP_TARGET   = "Liger"
    LIGER_LIBRARY_PATH = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/Frameworks
    LIGER_PLUGIN_PATH  = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/PlugIns
    TIGON_PLUGIN_PATH  = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/TigonPlugIns
    LIGER_LIBEXEC_PATH = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/Resources
    LIGER_DATA_PATH    = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/Resources
    LIGER_DOC_PATH     = $$LIGER_DATA_PATH/doc
    LIGER_BIN_PATH     = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/MacOS
    copydata = 1

    INSTALL_DATA_PATH    = $$LIGER_APP_PATH/$${LIGER_APP_TARGET}.app/Contents/Resources
} else {
    contains(TEMPLATE, vc.*):vcproj = 1
    LIGER_APP_TARGET   = liger
    LIGER_LIBRARY_PATH = $$LIGER_BUILD_TREE/bin
    LIGER_PLUGIN_PATH  = $$LIGER_BUILD_TREE/$$LIGER_LIBRARY_BASENAME/liger/plugins
    TIGON_PLUGIN_PATH  = $$LIGER_BUILD_TREE/$$LIGER_LIBRARY_BASENAME/tigon/plugins
    LIGER_LIBEXEC_PATH = $$LIGER_APP_PATH # FIXME
    LIGER_DATA_PATH    = $$LIGER_BUILD_TREE/share/liger
    LIGER_DOC_PATH     = $$LIGER_BUILD_TREE/share/doc
    LIGER_BIN_PATH     = $$LIGER_APP_PATH
    !isEqual(LIGER_SOURCE_TREE, $$LIGER_BUILD_TREE):copydata = 1

    INSTALL_DATA_PATH    = $$LIGER_BUILD_TREE/share/liger
}
TEST_BIN_PATH = $$LIGER_BUILD_TREE/tests/bin

INCLUDEPATH += \
    $$LIGER_BUILD_TREE/src \ # for <app/app_version.h>
    $$LIGER_SOURCE_TREE/src/libs \
    $$LIGER_SOURCE_TREE/tools

QTC_PLUGIN_DIRS = $$(QTC_PLUGIN_DIRS)
QTC_PLUGIN_DIRS = $$split(QTC_PLUGIN_DIRS, $$QMAKE_DIRLIST_SEP)
QTC_PLUGIN_DIRS += $$LIGER_SOURCE_TREE/src/plugins
for(dir, QTC_PLUGIN_DIRS) {
    INCLUDEPATH += $$dir
}

CONFIG += depend_includepath

LIBS += -L$$LIGER_LIBRARY_PATH
LIBS += -L$$TIGON_PLUGIN_PATH

!isEmpty(vcproj) {
    DEFINES += LIGER_LIBRARY_BASENAME=\"$$LIGER_LIBRARY_BASENAME\"
} else {
    DEFINES += LIGER_LIBRARY_BASENAME=\\\"$$LIGER_LIBRARY_BASENAME\\\"
}

# Note that the QT_NO_CAST_FROM_ASCII disables the QString(char*) constructor
# This is a design choice and makes sense but need time to port the current codebase
# to QLatin1String. So for now keep it commented out. TODO
DEFINES += LIGER QT_NO_CAST_TO_ASCII #QT_NO_CAST_FROM_ASCII
!macx:DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION

unix {
    CONFIG(debug, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/debug-shared
    CONFIG(release, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/release-shared

    CONFIG(debug, debug|release):MOC_DIR = $${OUT_PWD}/.moc/debug-shared
    CONFIG(release, debug|release):MOC_DIR = $${OUT_PWD}/.moc/release-shared

    RCC_DIR = $${OUT_PWD}/.rcc
    UI_DIR = $${OUT_PWD}/.uic
}

msvc {
    #Don't warn about sprintf, fopen etc being 'unsafe'
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CXXFLAGS_WARN_ON *= -w44996
    # Speed up startup time when debugging with cdb
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

qt:greaterThan(QT_MAJOR_VERSION, 4) {
    contains(QT, core): QT += concurrent
    contains(QT, gui): QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x040900
}

!isEmpty(QTC_PLUGIN_DEPENDS):LIBS *= -L$$LIGER_PLUGIN_PATH
# recursively resolve plugin deps
done_plugins =
for(ever) {
    isEmpty(QTC_PLUGIN_DEPENDS): \
        break()
    done_plugins += $$QTC_PLUGIN_DEPENDS
    for(dep, QTC_PLUGIN_DEPENDS) {
        dependencies_file =
        for(dir, QTC_PLUGIN_DIRS) {
            exists($$dir/$$dep/$${dep}_dependencies.pri) {
                dependencies_file = $$dir/$$dep/$${dep}_dependencies.pri
                break()
            }
        }
        isEmpty(dependencies_file): \
            error("Plugin dependency $$dep not found")
        include($$dependencies_file)
        LIBS += -l$$qtLibraryName($$QTC_PLUGIN_NAME)
    }
    QTC_PLUGIN_DEPENDS = $$unique(QTC_PLUGIN_DEPENDS)
    QTC_PLUGIN_DEPENDS -= $$unique(done_plugins)
}

# recursively resolve library deps
done_libs =
for(ever) {
    isEmpty(QTC_LIB_DEPENDS): \
        break()
    done_libs += $$QTC_LIB_DEPENDS
    for(dep, QTC_LIB_DEPENDS) {
        include($$PWD/src/libs/$$dep/$${dep}_dependencies.pri)
        LIBS += -l$$qtLibraryName($$QTC_LIB_NAME)
    }
    QTC_LIB_DEPENDS = $$unique(QTC_LIB_DEPENDS)
    QTC_LIB_DEPENDS -= $$unique(done_libs)
}
