# Check if Matlab exists, if so set MATLAB_ROOT_PATH

!contains(CONFIG, NO_MATLAB) {

linux-* {

  MATLAB_VERSIONS += "R2017b"
  MATLAB_VERSIONS += "R2018a"
  MATLAB_VERSIONS += "R2018b"
  MATLAB_VERSIONS += "R2019a"
  MATLAB_VERSIONS += "R2019b"
  MATLAB_VERSIONS += "R2020a"
  MATLAB_VERSIONS += "R2020b"

  TMP = $$system(whereis matlab)
  MATLABEXE = $$split(TMP," ")
  MATLABEXE = $$member(MATLABEXE, 1)

  !isEmpty(MATLABEXE) {
    MATLAB_ROOT_PATH_TMP = $$system(readlink $$MATLABEXE)
    MATLAB_ROOT_PATH_TMP = $$replace(MATLAB_ROOT_PATH_TMP, "/bin/matlab","")

    # check if the matlab version is supported
    MATLAB_SUPPORTED = 0
    M_VERSION = ""
    for(MATLAB_VERSION, MATLAB_VERSIONS) {
      if(contains(MATLAB_ROOT_PATH_TMP, .*$${MATLAB_VERSION}.*)) {
        MATLAB_SUPPORTED = 1
        M_VERSION = $$MATLAB_VERSION
      }
    }

    #message($$MATLAB_ROOT_PATH_TMP)

    isEqual(MATLAB_SUPPORTED, 1) {
      !isEmpty(MATLAB_ROOT_PATH_TMP) {
        MATLAB_ROOT_PATH = $$MATLAB_ROOT_PATH_TMP
        MATLAB_INCLUDE_PATH = $${MATLAB_ROOT_PATH}/extern/include
        MATLAB_LIB_PATH     = $${MATLAB_ROOT_PATH}/extern/bin/glnxa64
        DEFINES += MATLAB_API_CPP
        DEFINES += MATLAB_API
      }
    }

  } # end of isEmpty(MATLABEXE) {
} # end of linux


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

  # The Matlab version used by Liger is chosen from the following two lists, and
  # the criterion is to select the highest version number that is installed.
  # Both C and C++ APIs work with Matlab version R2017b, but it is currently
  # omitted from the C API to prevent any conflicts with the other API.

  # The following list contains the Matlab versions that are compatible with
  # the Matlab C API of Tigon.

  MATLAB_VERSIONS_C += "R2013b"
  MATLAB_VERSIONS_C += "R2014a"
  MATLAB_VERSIONS_C += "R2014b"
  MATLAB_VERSIONS_C += "R2015a"
  MATLAB_VERSIONS_C += "R2015b"
  MATLAB_VERSIONS_C += "R2016a"
  MATLAB_VERSIONS_C += "R2016b"
  MATLAB_VERSIONS_C += "R2017a"
#  MATLAB_VERSIONS_C += "R2017b"

  # The following list contains the Matlab versions that are compatible with
  # the Matlab C++ API of Tigon.

  MATLAB_VERSIONS_CPP += "R2017b"
  MATLAB_VERSIONS_CPP += "R2018a"
  MATLAB_VERSIONS_CPP += "R2018b"
  MATLAB_VERSIONS_CPP += "R2019a"
  MATLAB_VERSIONS_CPP += "R2019b"
  MATLAB_VERSIONS_CPP += "R2020a"
  MATLAB_VERSIONS_CPP += "R2020b"

  # Use command WHERE to find where MATLAB is installed
  # Use a silent call (/Q) that does not generate any output, just to check the error status
  # This prevents qmake from displying "INFO: Could not find files for the given pattern(s)." in case Matlab does not exists
  # The variable MATLAB_NOT_FOUND stores the error status, where 0 means OK (MATLAB found) and 1 means Error (MATLAB not found)
  TT = $$system(WHERE /Q matlab, true, MATLAB_NOT_FOUND)

  isEqual(MATLAB_NOT_FOUND, 0) { # if MATLAB has been found
    TMP = $$system(WHERE /F matlab) # now retrieve the actual path(s) with brackets (/F)
    MATLAB_LIST = $$split(TMP, "\" \"") # separate each MATLAB full path and return a list

    # To manually select a MATLAB version in case more than one is installed,
    # first uncomment the following three lines, then check the content of MATLAB_LIST.
    # The 0 in the second line selects the first directory in MATLAB_LIST, and
    # to select a different directory increase the number as appropriate.
#    message($$MATLAB_LIST)
#    MATLABTMP = $$member(MATLAB_LIST, 0)
#    MATLAB_LIST = $$MATLABTMP

    for(MATLABEXE, MATLAB_LIST) { # this loop goes through all paths that contains a matlab executable
      if(contains(MATLABEXE, .*matlab.exe.*)) { # selects only those paths with matlab.exe
         MATLABEXE = $$clean_path($$MATLABEXE) # replace \ by / (QT always uses / despite the OS)
         #message($$MATLABEXE)

         # check if the matlab version is supported by the C API
         for(MATLAB_VERSION, MATLAB_VERSIONS_C) {
           if(contains(MATLABEXE, .*$${MATLAB_VERSION}.*)) {
             SELECTED_MATLAB = $$MATLABEXE
             DEFINES *= MATLAB_API_C
           }
         }

         # check if the matlab version is supported bu the C++ API
         for(MATLAB_VERSION, MATLAB_VERSIONS_CPP) {
           if(contains(MATLABEXE, .*$${MATLAB_VERSION}.*)) {
             SELECTED_MATLAB = $$MATLABEXE
             DEFINES *= MATLAB_API_CPP
           } # if
         } # for
      } # if
    } # for

    contains(DEFINES, MATLAB_API_CPP) {
      if(exists($$SELECTED_MATLAB)) { # check if the executable exists
        SELECTED_MATLAB = $$replace(SELECTED_MATLAB, "\"","") # remove the character(")
        MATLABEXE_PATH = $$dirname(SELECTED_MATLAB) # extract the directory
        #message($$MATLABEXE)

        MATLAB_ROOT_PATH = $$replace(MATLABEXE_PATH, "/bin","") # go back one directory
        #message($$MATLAB_ROOT_PATH)
      } # if executable exists
    } # if matlab version is supported

    contains(DEFINES, MATLAB_API_C) {
      if(exists($$SELECTED_MATLAB)) { # check if the executable exists
        SELECTED_MATLAB = $$replace(SELECTED_MATLAB, "\"","") # remove the character(")
        MATLABEXE_PATH = $$dirname(SELECTED_MATLAB) # extract the directory
        #message($$MATLABEXE)

        MATLAB_ROOT_PATH = $$replace(MATLABEXE_PATH, "/bin","") # go back one directory
        #message($$MATLAB_ROOT_PATH)
      } # if executable exists
    } # if matlab version is supported

    !isEmpty(MATLAB_ROOT_PATH) {
      MATLAB_INCLUDE_PATH = $${MATLAB_ROOT_PATH}/extern/include
      MATLAB_LIB_PATHB    = $${MATLAB_ROOT_PATH}/extern/lib/win64/microsoft
      export(MATLAB_ROOT_PATH)
      export(MATLAB_INCLUDE_PATH)
      export(MATLAB_LIB_PATHB)
      DEFINES += MATLAB_API
    } # isEmpty

  } # isEqual MATLAB_FOUND
}
}
