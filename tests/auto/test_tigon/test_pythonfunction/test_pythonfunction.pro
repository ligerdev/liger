QTC_LIB_DEPENDS += tigon
include(../../../../src/libs/tigon/python.pri)
include(../../auto.pri)

SOURCES += \
    tst_pythonfunction.cpp

TEST_FILES = pythonfunc.py \
    pythonfunc_batch.py \
    pySchaffer.py \
    pythonImport.py \
    external_module.py \
    pythonfunction_workflow.lgr

copyToDir($$TEST_FILES, $$TEST_BIN_PATH)
