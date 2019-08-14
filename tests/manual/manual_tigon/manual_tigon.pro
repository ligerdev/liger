TEMPLATE = subdirs

SUBDIRS += \
    manual_evaluators \
    manual_moead \
    manual_nsgaiipsa \
    manual_nsgaii \
    manual_parego \
    manual_smsemoa \
    manual_moga

# Include matlab plugin
include(../../../matlabcheck.pri)
contains(DEFINES, MATLAB_API) {
  message("Include MATLAB mannual test")
  SUBDIRS += manual_matlabengine
} else {
  message("Does NOT include MATLAB mannual test")
}
