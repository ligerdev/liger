include(../../liger.pri)

TEMPLATE  = subdirs

# Plugins inherited from Qt Creator
SUBDIRS  = coreplugin
SUBDIRS += texteditor

# Liger plugins
SUBDIRS += designer
SUBDIRS += qtigon
SUBDIRS += plugincreator
SUBDIRS += visualisation
SUBDIRS += emailnotifier
SUBDIRS += matlabtemplate 
SUBDIRS += pythontemplate

for(p, SUBDIRS) {
    QTC_PLUGIN_DEPENDS =
    include($$p/$${p}_dependencies.pri)
    pv = $${p}.depends
    $$pv = $$QTC_PLUGIN_DEPENDS
}
