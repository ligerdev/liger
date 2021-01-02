/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include "designeditorplugin.h"
#include "designeditorfactory.h"
#include "designeditorw.h"
#include "designwizard.h"

#include "settingspage.h"
#include <QCoreApplication>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/designmode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/variablemanager.h>

#include <QTextBlock>
#include <QTextCursor>
#include <QTextFormat>
#include <QSharedData>
#include <QSharedDataPointer>
#include <designer/iengine/iport.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iengine.h>
#include <designer/iengine/nodeid.h>
#include <designer/iengine/nodeidfactory.h>
#include <designer/iengine/iprocessnodefactory.h>
#include <designer/widgets/designertoolbox.h>
//#include <designer/widgets/designersidetoolbar.h>

#include <QDebug>
#include <utils/qtcassert.h>
#include <QLibraryInfo>
#include <QTranslator>
#include <QtPlugin>

using namespace Core;
using namespace Designer::Internal;
using namespace Designer::Constants;

namespace Designer {
namespace Internal {
struct DesignEditorPluginPrivate {
    DesignEditorPluginPrivate();
    ~DesignEditorPluginPrivate();

    QMultiMap<int, QObject*> m_actionMap;

    QAction *m_runAction;
    QAction *m_stopRunAction;
    QAction *m_pauseAction;
    QAction *m_resumeAction;

    QAction *m_zoomin;
    QAction *m_zoomout;
    QAction *m_resetZooming;
    QAction *m_fitToWindow;

    Designer::RunMode m_runMode;
    QStringList m_arguments;
};

DesignEditorPluginPrivate::DesignEditorPluginPrivate() :
    m_runAction(0),
    m_stopRunAction(0),
    m_pauseAction(0),
    m_resumeAction(0),
    m_zoomin(0),
    m_zoomout(0),
    m_resetZooming(0),
    m_fitToWindow(0),
    m_runMode(Designer::NoRunMode)
{
}

DesignEditorPluginPrivate::~DesignEditorPluginPrivate()
{
    if(m_runAction)
        delete m_runAction;
    if(m_stopRunAction)
        delete m_stopRunAction;
    if(m_pauseAction)
        delete m_pauseAction;
    if(m_resumeAction)
        delete m_resumeAction;
    if(m_zoomin)
        delete m_zoomin;
    if(m_zoomout)
        delete m_zoomout;
    if(m_resetZooming)
        delete m_resetZooming;
    if(m_fitToWindow)
        delete m_fitToWindow;
}


} // namespace Internal
} // namespace Designer

DesignEditorPlugin *DesignEditorPlugin::m_instance = 0;

DesignEditorPlugin::DesignEditorPlugin()
    : m_actionSwitchSource(new QAction(tr("Switch Source/Form"), this)),
      d(new DesignEditorPluginPrivate)
{
    QTC_ASSERT(!m_instance, return);
    m_designerToolbox     = new DesignerToolBox();
    //m_designerSideToolbar = new DesignerSideToolBar();
    m_nodeIDFactory       = new NodeIDFactory();
    m_instance = this;
}

DesignEditorPlugin::~DesignEditorPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members - Remove in reverse order of registration!
    // Note that objects that have been added with: addAutoReleasedObject()
    // do not have to be manually removed.
    //delete m_designerSideToolbar;
    delete m_nodeIDFactory;

    DesignEditorW::deleteInstance();
    if(d)
        delete d;

    m_instance = 0;
}

DesignEditorPlugin *DesignEditorPlugin::instance()
{
    return m_instance;
}

////////////////////////////////////////////////////
//
// INHERITED FROM ExtensionSystem::Plugin
//
////////////////////////////////////////////////////
/*!
 * \brief initialize Initialise the plugin
 * \param arguments
 * \param errorMessage
 * \return true upon success
 */
bool DesignEditorPlugin::initialize(const QStringList &arguments, QString *error)
{
    Q_UNUSED(arguments)

    if (!MimeDatabase::addMimeTypes(QLatin1String(":/designer/Designer.mimetypes.xml"), error))
        return false;

    initializeTemplates();
    m_designEditorFactory  = new DesignEditorFactory();
    addAutoReleasedObject(m_designEditorFactory);
    m_settingsPageProvider = new SettingsPageProvider();
    addAutoReleasedObject(m_settingsPageProvider);
    // Ensure that loading designer translations is done before DesignEditorW is instantiated
    const QString locale = ICore::userInterfaceLanguage();
    if (!locale.isEmpty()) {
        QTranslator *qtr = new QTranslator(this);
        const QString &creatorTrPath =
                ICore::resourcePath() + QLatin1String("/translations");
        const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        const QString &trFile = QLatin1String("designer_") + locale;
        if (qtr->load(trFile, qtTrPath) || qtr->load(trFile, creatorTrPath))
            qApp->installTranslator(qtr);
    }

    //////////////// Designer /////////////////////
    addAutoReleasedObject(m_designerToolbox);

    //// Register Meta-types.
    //qRegisterMetaType<Designer::IPort>("Designer::IPort");
    //qRegisterMetaType<Designer::InputPort>("Designer::InputPort");
    //qRegisterMetaType<Designer::OutputPort>("Designer::OutputPort");
    //qRegisterMetaType<Designer::IProcessNode>("Designer::IProcessNode");
    //qRegisterMetaType<Designer::IEngine>("Designer::IEngine");
    //qRegisterMetaType<ProcessState>("ProcessState");
    //qRegisterMetaType<QTextBlock>("QTextBlock");
    //qRegisterMetaType<QTextCursor>("QTextCursor");
    //qRegisterMetaType<QTextFormat>("QTextFormat");
    //qRegisterMetaType<QSharedData>("QSharedData");
    //qRegisterMetaType<QStringList>("QStringList");

    Context designercontext(Core::Constants::C_DESIGN_MODE);

    ActionContainer *menubar =
        ActionManager::actionContainer(Core::Constants::MENU_BAR);

    // Execute workflow menu
    ActionContainer *mworkflow =
        ActionManager::createMenu(Constants::M_BUILDPROJECT);
    mworkflow->menu()->setTitle(tr("&Execute Workflow"));
    menubar->addMenu(mworkflow, Core::Constants::G_VIEW);

    //
    // Groups
    //
    mworkflow->appendGroup(Constants::G_RUN_STOP);
    mworkflow->appendGroup(Constants::G_RUN_RUN);

    ActionContainer *runMenu = ActionManager::createMenu(Constants::RUNMENUCONTEXTMENU);
    runMenu->setOnAllDisabledBehavior(ActionContainer::Hide);

    //
    // Separators
    //
    Command *cmd;
    mworkflow->addSeparator(designercontext, Constants::G_RUN_STOP);
    mworkflow->addSeparator(designercontext, Constants::G_RUN_RUN);

    //
    // Actions
    //

    // run action
    QIcon runIcon = QIcon(QLatin1String(Constants::ICON_RUN));
    runIcon.addFile(QLatin1String(Constants::ICON_RUN_SMALL));
    d->m_runAction = new QAction(runIcon, tr("Run"), this);
    cmd = ActionManager::registerAction(d->m_runAction, Constants::RUN, designercontext);
    cmd->setAttribute(Command::CA_UpdateText);

    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+R")));
    mworkflow->addAction(cmd, Constants::G_RUN_RUN);

    ModeManager::addAction(cmd->action(), Constants::P_ACTION_RUN);

    // pause action
    QIcon pauseIcon = QIcon(QLatin1String(Constants::ICON_PAUSE));
    pauseIcon.addFile(QLatin1String(Constants::ICON_PAUSE_SMALL));
    d->m_pauseAction = new QAction(pauseIcon, tr("Pause"), this);
    cmd = ActionManager::registerAction(d->m_pauseAction, Constants::PAUSE, designercontext);
    cmd->setAttribute(Command::CA_UpdateText);

    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+P")));
    mworkflow->addAction(cmd, Constants::G_RUN_RUN);

    // Hide the pause button when liger starts
    ModeManager::addAction(cmd->action(), Constants::P_ACTION_PAUSE);
    cmd->action()->setVisible(false);
    cmd->action()->setEnabled(false);
    d->m_pauseAction->setDisabled(true);
    d->m_pauseAction->setVisible(false);

    // stop action
    QIcon stopIcon = QIcon(QLatin1String(Constants::ICON_STOP));
    stopIcon.addFile(QLatin1String(Constants::ICON_STOP_SMALL));
    d->m_stopRunAction = new QAction(stopIcon, tr("Stop"), this);
    cmd = ActionManager::registerAction(d->m_stopRunAction, Constants::STOP, designercontext);
    cmd->setAttribute(Command::CA_UpdateText);

    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+E")));
    mworkflow->addAction(cmd, Constants::G_RUN_STOP);

    ModeManager::addAction(cmd->action(), Constants::P_ACTION_STOP);

    ///////////////////////

    // View workflow menu
    ActionContainer *mview =
        ActionManager::createMenu(Constants::M_VIEWPROJECT);
    mview->menu()->setTitle(tr("&View"));
    menubar->addMenu(mview, Core::Constants::G_VIEW);

    mview->appendGroup(Constants::G_VIEW_ZOOM);
    mview->appendGroup(Constants::G_VIEW_FIT);

    d->m_zoomin = new QAction(tr("Zoom In"), this);
    cmd = ActionManager::registerAction(d->m_zoomin, Constants::ZOOMIN, designercontext);
    cmd->setDefaultKeySequence(QKeySequence::ZoomIn);
    mview->addAction(cmd, Constants::G_VIEW_ZOOM);

    d->m_zoomout = new QAction(tr("Zoom Out"), this);
    cmd = ActionManager::registerAction(d->m_zoomout, Constants::ZOOMOUT, designercontext);
    cmd->setDefaultKeySequence(QKeySequence::ZoomOut);
    mview->addAction(cmd, Constants::G_VIEW_ZOOM);

    d->m_resetZooming = new QAction(tr("Reset Zooming"), this);
    cmd = ActionManager::registerAction(d->m_resetZooming, Constants::RESETZOOM, designercontext);
    cmd->setDefaultKeySequence(QKeySequence(tr("CTRL+0")));
    mview->addAction(cmd, Constants::G_VIEW_ZOOM);

    d->m_fitToWindow = new QAction(tr("Fit to Window"), this);
    cmd = ActionManager::registerAction(d->m_fitToWindow, Constants::FITTOWINDOW, designercontext);
    cmd->setDefaultKeySequence(QKeySequence(tr("CTRL+SHIFT+F")));
    mview->addAction(cmd, Constants::G_VIEW_FIT);

    mview->addSeparator(designercontext, Constants::G_VIEW_ZOOM);
    mview->addSeparator(designercontext, Constants::G_VIEW_FIT);

    ///////////////////////

    error->clear();
    return true;
}

void DesignEditorPlugin::extensionsInitialized()
{
    DesignMode::instance()->setDesignModeIsRequired();
    // 4) test and make sure everything works (undo, saving, editors, opening/closing multiple files, dirtiness etc)

    ActionContainer *mtools = ActionManager::actionContainer(Core::Constants::M_TOOLS);
    ActionContainer *mformtools = ActionManager::createMenu(M_FORMEDITOR);
    mformtools->menu()->setTitle(tr("Liger Designer"));
    mtools->addMenu(mformtools);
}

QAction* DesignEditorPlugin::runAction() const
{
    return d->m_runAction;
}

QAction *DesignEditorPlugin::pauseAction() const
{
    return d->m_pauseAction;
}

QAction* DesignEditorPlugin::stopRunAction() const
{
    return d->m_stopRunAction;
}

QAction *DesignEditorPlugin::zoominAction() const
{
    return d->m_zoomin;
}

QAction *DesignEditorPlugin::zoomoutAction() const
{
    return d->m_zoomout;
}

QAction *DesignEditorPlugin::resetZoomingAction() const
{
    return d->m_resetZooming;
}


QAction *DesignEditorPlugin::fitToWindowAction() const
{
    return d->m_fitToWindow;
}

////////////////////////////////////////////////////
//
// PRIVATE functions
//
////////////////////////////////////////////////////

void DesignEditorPlugin::initializeTemplates()
{
    IWizardFactory *wizard = new DesignWizard;
    wizard->setWizardKind(IWizardFactory::FileWizard);
    wizard->setCategory(QLatin1String(Designer::Constants::WIZARD_CATEGORY));
    wizard->setDisplayCategory(QCoreApplication::translate("Designer", Designer::Constants::WIZARD_TR_CATEGORY));
    wizard->setDisplayName(tr("Liger Optimization Workflow"));
    wizard->setId(QLatin1String("D.OptimizationWorkflow"));
    wizard->setDescription(tr("Creates a Liger optimization workflow. "));
    addAutoReleasedObject(wizard);
}

// Find out current existing editor file
QString DesignEditorPlugin::currentFile()
{
    if (const IDocument *document = EditorManager::currentDocument()) {
        const QString fileName = document->filePath();
        if (!fileName.isEmpty() && QFileInfo(fileName).isFile())
            return fileName;
    }
    return QString();
}

void DesignEditorPlugin::swapRunButtonToPause()
{
    d->m_runAction->setDisabled(true);
    d->m_runAction->setVisible(false);
    d->m_pauseAction->setVisible(true);
    d->m_pauseAction->setDisabled(false);
}

void DesignEditorPlugin::resetRunButton()
{
    d->m_runAction->setDisabled(false);
    d->m_runAction->setVisible(true);
    d->m_pauseAction->setVisible(false);
    d->m_pauseAction->setDisabled(true);
}

Q_EXPORT_PLUGIN(DesignEditorPlugin)
