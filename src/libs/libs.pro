include(../../liger.pri)
TEMPLATE  = subdirs
# aggregation and extensionsystem are directly in src.pro
# because of dependencies of app.
SUBDIRS   = \
    aggregation \
    extensionsystem \
    utils \
    libavoid \
    botan \
    eigen \
    fann  \
    json  \
    tinyxml2 \
    tigon

# Include matlab plugin
contains(DEFINES, MATLAB_API) {
  message("Include MATLAB plugin")
  SUBDIRS += matlabplugin
} else {
  message("Does NOT include MATLAB plugin")
}

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
