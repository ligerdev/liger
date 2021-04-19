TEMPLATE = subdirs

SUBDIRS = \
    test_baseoperations \
    test_interpolators  \
    test_problemgenerator \
    test_tigonutilities \
    test_uncertainties \
    test_tigonengine \
    test_nsgaii \
    test_moead \
    test_exception \
    test_log \
    test_ctp \
    test_parego \
    test_sparego \
    test_convergence \
    test_pythonfunction \
    test_pluginsystem \
    test_simplexlattice \
    test_smsemoa \
    test_fann \
    test_statistics \
    test_moga \
    test_nsgaiii

# Include matlab plugin
include(../../../matlabcheck.pri)
contains(DEFINES, MATLAB_API) {
  message("Include MATLAB plugin test")
  SUBDIRS += test_matlabintegration
} else {
  message("Does NOT include MATLAB plugin test")
}
