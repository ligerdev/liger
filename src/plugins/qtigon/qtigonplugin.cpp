/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#include <designer/iengine/iengine.h>
#include <qtigon/qtigonplugin.h>

// Factories
#include <designer/iengine/iprocessnodefactory.h>
#include <qtigon/engine/qtigonengine.h>
#include <qtigon/engine/qtigonenginefactory.h>
#include <qtigon/masterstartnodefactory.h>
#include <qtigon/masterendnodefactory.h>
#include <qtigon/operators/problemgenerator/qopprobgeneratornodefactory.h>
#include <qtigon/operators/initialisations/qoprandominitnodefactory.h>
#include <qtigon/operators/evaluators/qopvalidationnodefactory.h>
#include <qtigon/operators/initialisations/qoplhsinitnodefactory.h>
#include <qtigon/operators/evaluators/qopevaluatornodefactory.h>
#include <qtigon/operators/evaluators/qopbatchevaluatornodefactory.h>
#include <qtigon/algorithms/nsgaiipsa/qalgonsgaiipsanodefactory.h>
#include <qtigon/algorithms/nsgaii/qalgonsgaiinodefactory.h>
#include <qtigon/algorithms/parego/qalgoparegonodefactory.h>
#include <qtigon/algorithms/sparego/qalgosparegonodefactory.h>
#include <qtigon/algorithms/moead/qalgomoeadnodefactory.h>
#include <qtigon/operators/initialisations/qopsparegoinitnodefactory.h>
#include <qtigon/operators/initialisations/qopuserdefinedinitnodefactory.h>
#include <qtigon/algorithms/smsemoa/qalgosmsemoanodefactory.h>

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <tigon/Core/PluginManager.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

#include <QtCore/QtPlugin>

namespace QTigon {
namespace Internal {

QTigonPlugin::QTigonPlugin()
{
    // Create your members
}

QTigonPlugin::~QTigonPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members - Remove in reverse order of registration!
    // Note that objects that have been added with: addAutoReleasedObject()
    // do not have to be manually removed.
}

bool QTigonPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    using namespace Designer;

    ///[] Register objects in the plugin manager's object pool
    // Engines
    QTigonEngineFactory* engineFactory = new QTigonEngineFactory();
    addAutoReleasedObject(engineFactory);
    IEngine* engine = engineFactory->createEngine();
    addAutoReleasedObject(engine);

    // Master start/end nodes
    addAutoReleasedObject(new MasterStartNodeFactory);
    addAutoReleasedObject(new MasterEndNodeFactory);

    // Operators
    addAutoReleasedObject(new QOpProbGeneratorNodeFactory);
    addAutoReleasedObject(new QOpRandomInitNodeFactory);
    addAutoReleasedObject(new QOpValidationNodeFactory);
    addAutoReleasedObject(new QOpLHSInitNodeFactory);
    addAutoReleasedObject(new QOpEvaluatorNodeFactory);
    addAutoReleasedObject(new QOpBatchEvaluatorNodeFactory);
    addAutoReleasedObject(new QOpUserDefinedInitNodeFactory);
    addAutoReleasedObject(new QOpSparegoInitNodeFactory);

    // Algorithms
    addAutoReleasedObject(new QAlgoNSGAIIPSANodeFactory);
    addAutoReleasedObject(new QAlgoNSGAIINodeFactory);
    addAutoReleasedObject(new QAlgoParEGONodeFactory);
    addAutoReleasedObject(new QAlgosParEGONodeFactory);
    addAutoReleasedObject(new QAlgoMOEADNodeFactory);
    addAutoReleasedObject(new QAlgoSMSEMOANodeFactory);

    ///[] Load Tigon plugins
    QDir cpath = QCoreApplication::applicationDirPath();
    cpath.cdUp();
#ifdef __APPLE__
    cpath.cd("TigonPlugIns");
#else
    cpath.cd("lib/tigon/plugins");
#endif

    QString pluginPath = cpath.path();
    Tigon::PluginManager::instance()->definePath(pluginPath.toStdString());
    Tigon::PluginManager::instance()->loadPlugins();

    return true;
}

void QTigonPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
}

ExtensionSystem::IPlugin::ShutdownFlag QTigonPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void QTigonPlugin::triggerAction()
{
    QMessageBox::information(Core::ICore::instance()->mainWindow(),
                             tr("Action triggered"),
                             tr("This is an action from QTigon."));
}

}  // namespace Internal
}  // namespace QTigon

Q_EXPORT_PLUGIN(QTigon)

