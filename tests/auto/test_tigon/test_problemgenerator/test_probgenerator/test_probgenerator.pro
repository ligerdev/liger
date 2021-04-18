# NOTE: If you are trying to debug (i.e. step through) the tests, make sure you disable
# QML in the Debugger settings. Namely, go to Projects locate Debugger Settings and
# uncheck "Enable QML".

# Make sure the dependencies are included before qttest.pri, otherwise they are not parsed.
QTC_LIB_DEPENDS += tigon
include(../../../auto.pri)

SOURCES += \
    tst_problemgenerator.cpp

#RESOURCES += \
#    test_problemgenerator_resource.qrc


TEST_FILES = unittest_grouped_data.xml

copyToDir($$TEST_FILES, $$TEST_BIN_PATH)
