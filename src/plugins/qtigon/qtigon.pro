# After adding classes that inherit QObject and have Q_OBJECT functionality,
# for example signals or slots, you have to "force" qtcreator to reparse the
# subproject (e.g. make a null change to the *.pro file) so that the makefiles
# are regenerated and new MOC files are created for the new or modified classes.

CONFIG  += exceptions
QT += xml svg widgets
DEFINES += QTIGON_LIBRARY

include(../../libs/boost.pri)
include(../../ligerplugin.pri)

linux-* {

}

macx {

}

win32 {

}

SOURCES += \ 
    qtigonplugin.cpp \
    masterendnode.cpp \
    masterendnodefactory.cpp \
    masterstartnode.cpp \
    masterstartnodefactory.cpp \
    dialogs/endnodedialog.cpp \
    operators/problemgenerator/qopprobgeneratornode.cpp \
    operators/problemgenerator/qopprobgeneratornodefactory.cpp \
    engine/qtigonengine.cpp \
    engine/qtigonenginefactory.cpp \
    algorithms/nsgaiipsa/qalgonsgaiipsanode.cpp \
    algorithms/nsgaiipsa/qalgonsgaiipsanodefactory.cpp \
    dialogs/qalgorithmdialog.cpp \
    dialogs/qoperatordiag.cpp \
    dialogs/qopprobformulationnodeform.cpp \
    dialogs/functionloaderdialog.cpp \
    operators/initialisations/qoprandominitnode.cpp \
    operators/initialisations/qoprandominitnodefactory.cpp  \
    algorithms/nsgaii/qalgonsgaiinode.cpp \
    algorithms/nsgaii/qalgonsgaiinodefactory.cpp \
    algorithms/moead/qalgomoeadnode.cpp \
    algorithms/moead/qalgomoeadnodefactory.cpp  \
    algorithms/smsemoa/qalgosmsemoanode.cpp \
    algorithms/smsemoa/qalgosmsemoanodefactory.cpp \
    dialogs/numinputoutputform.cpp \
    dialogs/inputpropertiesdialog.cpp \
    operators/evaluators/qopvalidationnode.cpp \
    operators/evaluators/qopvalidationnodefactory.cpp \
    operators/initialisations/qoplhsinitnode.cpp \
    operators/initialisations/qoplhsinitnodefactory.cpp \
    operators/evaluators/qopevaluatornode.cpp \
    operators/evaluators/qopevaluatornodefactory.cpp \
    operators/evaluators/qopbatchevaluatornode.cpp \
    operators/evaluators/qopbatchevaluatornodefactory.cpp \
    dialogs/comboboxdelegate.cpp \
    dialogs/inputoutputcheckingform.cpp \
    dialogs/customlistwidget.cpp \
    dialogs/outputpropertiesdialog.cpp \
    operators/initialisations/qopuserdefinedinitnode.cpp \
    operators/initialisations/qopuserdefinedinitnodefactory.cpp \
    dialogs/populationviewer.cpp \
    dialogs/qoperatordiagtabitem.cpp \
    algorithms/parego/qalgoparegonode.cpp \
    algorithms/parego/qalgoparegonodefactory.cpp \
    qtigonutils.cpp \
    dialogs/linkingvariablegrouppropertiesdialog.cpp \
    dialogs/startnodedialog.cpp

HEADERS += \ 
    qtigon_global.h \
    qtigonconstants.h \
    qtigonplugin.h \
    masterendnode.h \
    masterendnodefactory.h \
    masterstartnode.h \
    masterstartnodefactory.h \
    dialogs/endnodedialog.h \
    operators/problemgenerator/qopprobgeneratornode.h \
    operators/problemgenerator/qopprobgeneratornodefactory.h \
    engine/qtigonengine.h \
    engine/qtigonenginefactory.h \
    algorithms/nsgaiipsa/qalgonsgaiipsanode.h \
    algorithms/nsgaiipsa/qalgonsgaiipsanodefactory.h \
    dialogs/qalgorithmdialog.h \
    dialogs/qoperatordiag.h \
    dialogs/qopprobformulationnodeform.h \
    dialogs/functionloaderdialog.h  \
    operators/initialisations/qoprandominitnode.h \
    operators/initialisations/qoprandominitnodefactory.h  \
    algorithms/nsgaii/qalgonsgaiinode.h \
    algorithms/nsgaii/qalgonsgaiinodefactory.h \
    algorithms/moead/qalgomoeadnode.h \
    algorithms/moead/qalgomoeadnodefactory.h  \
    algorithms/smsemoa/qalgosmsemoanode.h \
    algorithms/smsemoa/qalgosmsemoanodefactory.h \
    dialogs/numinputoutputform.h \
    dialogs/inputpropertiesdialog.h \
    operators/problemgenerator/inputoutputprivatedata.h \
    operators/evaluators/qopvalidationnode.h \
    operators/evaluators/qopvalidationnodefactory.h \
    operators/initialisations/qoplhsinitnode.h \
    operators/initialisations/qoplhsinitnodefactory.h \
    operators/evaluators/qopevaluatornode.h \
    operators/evaluators/qopevaluatornodefactory.h \
    operators/evaluators/qopbatchevaluatornode.h \
    operators/evaluators/qopbatchevaluatornodefactory.h \
    dialogs/comboboxdelegate.h \
    dialogs/inputoutputcheckingform.h \
    dialogs/customlistwidget.h \
    dialogs/outputpropertiesdialog.h \
    operators/initialisations/qopuserdefinedinitnode.h \
    operators/initialisations/qopuserdefinedinitnodefactory.h \
    dialogs/populationviewer.h \
    dialogs/qoperatordiagtabitem.h \
    algorithms/parego/qalgoparegonode.h \
    algorithms/parego/qalgoparegonodefactory.h \
    qtigonutils.h \
    dialogs/linkingvariablegrouppropertiesdialog.h \
    dialogs/startnodedialog.h

OTHER_FILES = QTigon.pluginspec.in

RESOURCES += \
    qtigon.qrc

FORMS += \ 
    dialogs/endnodedialog.ui \
    dialogs/qalgorithmdialog.ui \
    dialogs/qoperatordiag.ui \
    dialogs/qopprobformulationnodeform.ui \
    dialogs/functionloaderdialog.ui \
    dialogs/numinputoutputform.ui \
    dialogs/inputpropertiesdialog.ui \
    dialogs/inputoutputcheckingform.ui \
    dialogs/outputpropertiesdialog.ui \
    dialogs/populationviewer.ui \
    dialogs/linkingvariablegrouppropertiesdialog.ui \
    dialogs/startnodedialog.ui
