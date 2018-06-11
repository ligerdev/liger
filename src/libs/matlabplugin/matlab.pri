!contains(CONFIG, NO_MATLAB) {
include(../../../matlabcheck.pri)

# Check if Matlab exists, if so set MATLAB_ROOT_PATH
#!exists(MATLAB_ROOT_PATH) {
linux-* {
#    !isEmpty(MATLAB_ROOT_PATH) {
#      LIBS += -L$$MATLAB_LIB_PATH/ -leng -lmx -lm -lstdc++
#      INCLUDEPATH += $$MATLAB_INCLUDE_PATH
#      DEPENDPATH  += $$MATLAB_INCLUDE_PATH

#      QMAKE_RPATHDIR += $$(QTDIR)/lib
#      QMAKE_RPATHDIR += $$MATLAB_LIB_PATH
#      QMAKE_RPATHDIR += $${MATLAB_ROOT_PATH}/sys/os/glnxa64
#    }

#    isEmpty(MATLAB_ROOT_PATH) {
#      message("A compatible Matlab installation could not be found.")
#    }
#  }
}

macx {
  isEmpty(MATLAB_ROOT_PATH) {
    message("A compatible Matlab installation could not be found.")
  }
  !isEmpty(MATLAB_ROOT_PATH) {
    LIBS += $$MATLAB_LIB_PATH/libmx.dylib
    LIBS += $$MATLAB_LIB_PATH/libmex.dylib
    LIBS += $$MATLAB_LIB_PATH/libmat.dylib
    LIBS += $$MATLAB_LIB_PATH/libeng.dylib
    INCLUDEPATH += $$MATLAB_INCLUDE_PATH
    DEPENDPATH  += $$MATLAB_INCLUDE_PATH

    QMAKE_RPATHDIR += $$MATLAB_LIB_PATH
    QMAKE_RPATHDIR += $${MATLAB_ROOT_PATH}/sys/os/maci64
  }
}

win32 {
  isEmpty(MATLAB_ROOT_PATH) {
    message("A compatible Matlab installation could not be found.")
  }
  !isEmpty(MATLAB_ROOT_PATH) {
    LIBS += -L$$MATLAB_LIB_PATHB -llibmx -llibmex -llibmat -llibeng
    INCLUDEPATH += $$MATLAB_INCLUDE_PATH
    DEPENDPATH  += $$MATLAB_INCLUDE_PATH
  }
}
}
