# To disable MATLAB support, pass "CONFIG+=NO_MATLAB" to qmake as an argument
# To disable Python support, pass "DISABLE_PYTHON=yes" to qmake as an argument
# To disable tests, pass "CONFIG += NO_TESTS" to qmake as an argument

include(liger.pri)

# Qt version check
!minQtVersion(5, 6, 0) {
    message("Cannot build Liger with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.6.0.")
}

# Documentation
include(doc/doc.pri)

TEMPLATE  = subdirs
CONFIG   += ordered
CONFIG   += silent

SUBDIRS = src share
unix:!macx:!isEmpty(copydata): SUBDIRS += bin

!contains(CONFIG, NO_TESTS) {
    SUBDIRS += tests
}
macx: PLATFORM = "mac"
else:win32: PLATFORM = "windows"
else:linux-*: PLATFORM = "linux-$${ARCHITECTURE}"
else: PLATFORM = "unknown"

BASENAME = $$(INSTALL_BASENAME)
isEmpty(BASENAME): BASENAME = liger-$${PLATFORM}$(INSTALL_EDITION)-$${LIGER_VERSION}$(INSTALL_POSTFIX)
macx:INSTALLER_NAME = "liger-$${LIGER_VERSION}"
else:INSTALLER_NAME = "$${BASENAME}"

# get Qt directory
QT_DIRECTORY=$$[QT_HOST_PREFIX]

win32 {
     APPBUNDLE = "$$OUT_PWD\bin"
     deployqt.commands = $$PWD\dev\WindowsBuild\deployqtHelper_win.bat "$$APPBUNDLE" "$${QT_DIRECTORY}\bin"
     # TODO - Yiming: command for creating installer; leave it blank for now
     deploy.commands = ""
     deployqt.commands ~= s,/,\\,g
     deploy.commands ~= s,/,\\,g
     deploy.depends = deployqt
     QMAKE_EXTRA_TARGETS += deploy deployqt
}

macx {
    APPBUNDLE = "$$OUT_PWD/bin/Liger.app"
    BINDIST_SOURCE = "$$OUT_PWD/bin/Liger.app"
    BINDIST_INSTALLER_SOURCE = $$BINDIST_SOURCE
    deployqt.commands = $$PWD/dev/MacOSXBuild/deployqtHelper_mac.sh $$APPBUNDLE "$${QT_DIRECTORY}/bin"
    deploy.commands = $$PWD/dev/MacOSXBuild/makedmg.sh $$OUT_PWD/bin liger-$${LIGER_VERSION}.dmg
    deploy.depends = deployqt
    QMAKE_EXTRA_TARGETS += deploy deployqt
}

TMP = $$(BOOST_LIB)
linux-* {
    deploy.commands = $$PWD/dev/LinuxBuild/linux_create_build.sh $${OUT_PWD} $${OUT_PWD} $${QT_DIRECTORY} $${PWD} $$TMP
    QMAKE_EXTRA_TARGETS += deploy
}
