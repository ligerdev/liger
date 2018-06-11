# Check if Matlab exists, if so set MATLAB_ROOT_PATH
!contains(CONFIG, NO_MATLAB) {
linux-* {
#  TMP = $$system(whereis matlab)
#  MATLABEXE = $$split(TMP," ")
#  MATLABEXE = $$member(MATLABEXE, 1)
# !isEmpty(MATLABEXE) {
#    MATLAB_ROOT_PATH = $$system(readlink $$MATLABEXE)
#    MATLAB_ROOT_PATH = $$replace(MATLAB_ROOT_PATH, "/bin/matlab","")

#    !isEmpty(MATLAB_ROOT_PATH) {
#      MATLAB_INCLUDE_PATH = $${MATLAB_ROOT_PATH}/extern/include
#      MATLAB_LIB_PATH     = $${MATLAB_ROOT_PATH}/bin/glnxa64
#      DEFINES += MATLAB_API
#    }
#  }
}

macx {
  MATLAB_PATHS += /Applications/MATLAB_R2013a.app
  MATLAB_PATHS += /Applications/MATLAB_R2013b.app
  MATLAB_PATHS += /Applications/MATLAB_R2014a.app
  MATLAB_PATHS += /Applications/MATLAB_R2014b.app
  MATLAB_PATHS += /Applications/MATLAB_R2015a.app
  MATLAB_PATHS += /Applications/MATLAB_R2015b.app
  MATLAB_PATHS += /Applications/MATLAB_R2016a.app
  MATLAB_PATHS += /Applications/MATLAB_R2017a.app

  for(CURPATH, MATLAB_PATHS) {
    if(exists($$CURPATH)) {
      MATLAB_ROOT_PATH    = $$CURPATH
      MATLAB_INCLUDE_PATH = $${MATLAB_ROOT_PATH}/extern/include
      MATLAB_LIB_PATH     = $${MATLAB_ROOT_PATH}/bin/maci64
      export(MATLAB_ROOT_PATH)
      export(MATLAB_INCLUDE_PATH)
      export(MATLAB_LIB_PATH)
    }
  }

  !isEmpty(MATLAB_ROOT_PATH) {
    DEFINES += MATLAB_API
  }
}

win32 {
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2013b"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2014a"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2014b"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2015a"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2015b"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2016a"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2016b"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2017a"
  MATLAB_PATHS += "C:/Program Files/MATLAB/R2017b"

  for(CURPATH, MATLAB_PATHS) {
    if(exists($$CURPATH)) {
      MATLAB_ROOT_PATH    = $$CURPATH
      MATLAB_INCLUDE_PATH = $${MATLAB_ROOT_PATH}/extern/include
      MATLAB_LIB_PATHA    = $${MATLAB_ROOT_PATH}/bin/win32
      MATLAB_LIB_PATHB    = $${MATLAB_ROOT_PATH}/extern/lib/win64/microsoft

      export(MATLAB_ROOT_PATH)
      export(MATLAB_INCLUDE_PATH)
      export(MATLAB_LIB_PATHA)
      export(MATLAB_LIB_PATHB)
    }
  }
  !isEmpty(MATLAB_ROOT_PATH) {
    DEFINES += MATLAB_API
  }
}
}
