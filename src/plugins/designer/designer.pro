DEFINES += DESIGNER_LIBRARY

include(../../libs/boost.pri)
include(../../ligerplugin.pri)

QT += xml svg printsupport concurrent
CONFIG += exceptions
CONFIG += include_source_dir # For the highlighter autotest.

HEADERS += \
        designerconstants.h \
        settingspage.h \
        editorwidget.h \
        settingsmanager.h \
        designercontext.h \
        editordata.h \
        designerxmleditorwidget.h \
        # iengine/processengine.h \
        iengine/iprocessnode.h \
        iengine/iengine.h \
        iengine/ienginefactory.h \
        iengine/iprocessnodefactory.h \
        iengine/iport.h \
        iengine/iportlink.h \
        iengine/portlink.h \
        iengine/inputport.h \
        iengine/outputport.h \
        iengine/portlinkfactory.h \
        iengine/nodeid.h \
        iengine/nodeidfactory.h \
        # iengine/globalspace.h \
        # widgets/designersidetoolbar.h \
        widgets/designertoolbox.h \
        widgets/customtoolbox.h \
        designeditorfactory.h \
        designeditorplugin.h \
        designeditorstack.h \
        designeditorw.h \
        designwindoweditor.h \
        # designwindowfile.h \
        designwizard.h \
        designwizarddialog.h \
        ldesigner/ldesigner.h \
        ldesigner/ldesignerwidget.h \
        ldesigner/ldesigndocument.h \
        ldesigner/ldesignerfactory.h \
        ldesigner/widgethost.h \
        ldesigner/designerwidgetcontainer.h \
        iengine/icountinglabel.h \
    iengine/ilogmanager.h \
    designer_global.h

SOURCES += \
        settingspage.cpp \
        editorwidget.cpp \
        settingsmanager.cpp \
        designercontext.cpp \
        designerxmleditorwidget.cpp \
        #iengine/processengine.cpp \
        iengine/ienginefactory.cpp \
        iengine/iprocessnodefactory.cpp \
        iengine/iprocessnode.cpp \
        iengine/iengine.cpp \
        iengine/iport.cpp \
        iengine/portlink.cpp \
        iengine/inputport.cpp \
        iengine/outputport.cpp \
        iengine/portlinkfactory.cpp \
        iengine/nodeid.cpp \
        iengine/nodeidfactory.cpp \
        # iengine/globalspace.cpp \
        # widgets/designersidetoolbar.cpp \
        widgets/designertoolbox.cpp \
        widgets/customtoolbox.cpp \
        designeditorfactory.cpp \
        designeditorplugin.cpp \
        designeditorstack.cpp \
        designeditorw.cpp \
        designwindoweditor.cpp \
        # designwindowfile.cpp \
        designwizard.cpp \
        designwizarddialog.cpp \
        ldesigner/ldesignerwidget.cpp \
        ldesigner/ldesigndocument.cpp \
        ldesigner/ldesigner.cpp \
        ldesigner/ldesignerfactory.cpp \
        ldesigner/widgethost.cpp \
        ldesigner/designerwidgetcontainer.cpp \
        iengine/icountinglabel.cpp \
    iengine/ilogmanager.cpp

RESOURCES += designer.qrc

OTHER_FILES +=
