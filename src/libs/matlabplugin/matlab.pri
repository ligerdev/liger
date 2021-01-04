
# Defines the include and library paths (and others) for Matlab

!contains(CONFIG, NO_MATLAB) {

include(../../../matlabcheck.pri)

linux-* {
  isEmpty(MATLAB_ROOT_PATH) {
    message("A compatible Matlab installation could not be found.")
  }
  !isEmpty(MATLAB_ROOT_PATH) {

    message("A compatible Matlab installation has been found in $$MATLAB_ROOT_PATH")

    LIBS += -L$$MATLAB_LIB_PATH/ -pthread -lMatlabDataArray -lMatlabEngine -lstdc++
    INCLUDEPATH += $$MATLAB_INCLUDE_PATH
    DEPENDPATH  += $$MATLAB_INCLUDE_PATH

    QMAKE_RPATHDIR += $$(QTDIR)/lib
    QMAKE_RPATHDIR += $$MATLAB_LIB_PATH
    QMAKE_RPATHDIR += $${MATLAB_ROOT_PATH}/sys/os/glnxa64
  }
} # end linux

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
} # end macx

win32 {
  isEmpty(MATLAB_ROOT_PATH) {
    message("A compatible Matlab installation could not be found.")
  }
  !isEmpty(MATLAB_ROOT_PATH) {

    message("A compatible Matlab installation has been found in $$MATLAB_ROOT_PATH")

    contains(DEFINES, MATLAB_API_CPP) {
      LIBS += -L$$MATLAB_LIB_PATHB
      LIBS += $$MATLAB_LIB_PATHB/libMatlabEngine.lib
      LIBS += $$MATLAB_LIB_PATHB/libMatlabDataArray.lib
    }

    contains(DEFINES, MATLAB_API_C) {
      LIBS += -L$$MATLAB_LIB_PATHB -llibmx -llibmex -llibmat -llibeng
    }

    INCLUDEPATH += $$MATLAB_INCLUDE_PATH
    DEPENDPATH  += $$MATLAB_INCLUDE_PATH
  }
} # end win32

} # check if NO_MATLAB is defined
