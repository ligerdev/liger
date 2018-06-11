QTC_LIB_DEPENDS += tigon matlabplugin
include(../../../../src/libs/matlabplugin/matlab.pri)
include(../../auto.pri)

# Make sure you enable compiler optimization, otherwise Eigen becomes really slow
# when operating on dynamically allocated matrices!
macx {

}
win32 {
#QMAKE_CXXFLAGS = /O2 /F1000000000000000
}

DEFINES += LIGER_TIGON_LIB

SOURCES += \
    tst_matlabintegration.cpp

MATLAB_TEST_FILES += \
    test_mfunctions/test_properties_parsing.m  \
    test_mfunctions/test1_matlabTigonWrapper.m \
    test_mfunctions/test2_matlabTigonWrapper.m \
    test_mfunctions/test3_tigonBadStruct.m \
    test_mfunctions/test4_tigonBadStruct.m \
    test_mfunctions/test5_tigonBadStruct.m \
    test_mfunctions/test6_tigonBadStruct.m \
    test_mfunctions/test7_tigonBadStruct.m \
    test_mfunctions/test8_tigonBadStruct.m \
    test_mfunctions/test_matlabfunction_workflow.lgr \
    test_mfunctions/DTLZ1.m

copyToDir($$MATLAB_TEST_FILES, $$TEST_BIN_PATH)
