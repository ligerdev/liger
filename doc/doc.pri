# Document of Liger
# Please use doxygen
#
# Dependencies:
#   1. doxygen
#   2. dot (graphviz)
OTHER_FILES += doxygen/LigerDoxyFile

SET_DOXYGEN_PARAMS = "echo INPUT  = \"$$LIGER_SOURCE_TREE/src\" & \
                      echo EXCLUDE = \"$$LIGER_SOURCE_TREE/src/libs/boost\"  \
                           \"$$LIGER_SOURCE_TREE/src/libs/eigen\"  \
                           \"$$LIGER_SOURCE_TREE/src/libs/botan\"  \
                           \"$$LIGER_SOURCE_TREE/src/plugins/htmleditor\" \
                           \"$$LIGER_SOURCE_TREE/src/plugins/texteditor\" \
                           \"$$LIGER_SOURCE_TREE/src/plugins/find\" "

# Doxygen
win32 {
  # Check if doxygen is installed
  DOXYGEN_BIN = $$system(where doxygen)
  # Check if graphviz is installed
  DOT_BIN     = $$system(where dot)
  !isEmpty(DOT_BIN) {
      SET_DOXYGEN_PARAMS = $${SET_DOXYGEN_PARAMS} & echo HAVE_DOT = YES
  }

  !isEmpty(DOXYGEN_BIN) {
    doc.commands = "( type \"$$PWD\doxygen\LigerDoxyFile\" & $$SET_DOXYGEN_PARAMS ) | \"$$DOXYGEN_BIN\" -"
    doc.commands ~= s,/,\\,g
  }
}

macx {
  # Check if doxygen is installed
  DOXYGEN_BIN = $$system(which doxygen)
  DOT_BIN     = $$system(which dot)
  !isEmpty(DOT_BIN) {
      SET_DOXYGEN_PARAMS = $${SET_DOXYGEN_PARAMS} & echo HAVE_DOT = YES
  }
  !isEmpty(DOXYGEN_BIN) {
    doc.commands = "( cat \"$$PWD/doxygen/LigerDoxyFile\" ; $$SET_DOXYGEN_PARAMS ) | \"$$DOXYGEN_BIN\" -"
  }
}

linux-* {
  # Check if doxygen is installed
  DOXYGEN_BIN = $$system(which doxygen)
  DOT_BIN     = $$system(which dot)
  !isEmpty(DOT_BIN) {
      SET_DOXYGEN_PARAMS = $${SET_DOXYGEN_PARAMS} & echo HAVE_DOT = YES
  }
  !isEmpty(DOXYGEN_BIN) {
    doc.commands = "( cat \"$$PWD/doxygen/LigerDoxyFile\" ; $$SET_DOXYGEN_PARAMS ) | \"$$DOXYGEN_BIN\" -"
  }
}

QMAKE_EXTRA_TARGETS += doc
