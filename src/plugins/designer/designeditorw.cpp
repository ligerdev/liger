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
#include "designeditorw.h"
#include "designwindoweditor.h"
#include "settingsmanager.h"
#include "settingspage.h"
#include "editorwidget.h"
#include "editordata.h"
#include "designerxmleditorwidget.h"
#include "designercontext.h"
#include <designer/ldesigner/widgethost.h>

#include <ldesigner/ldesignerfactory.h>
#include <ldesigner/ldesigner.h>

#include <coreplugin/editortoolbar.h>
#include <coreplugin/designmode.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/helpmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/minisplitter.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/outputpane.h>
#include <texteditor/texteditorsettings.h>
#include <utils/qtcassert.h>

//#include <designer/widgets/designersidetoolbar.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCursor>
#include <QDockWidget>
#include <QMessageBox>
#include <QKeySequence>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QStyle>
#include <QToolBar>
#include <QVBoxLayout>

#include <QDebug>
#include <QSettings>
#include <QSignalMapper>
#include <QPluginLoader>
#include <QTime>

// For DesignerToolBar
#include <designer/designeditorplugin.h>
#include <extensionsystem/pluginmanager.h>
#include <qobject.h>
#include <QWidget>

static const char settingsGroupC[] = "Designer";

/* Actions of the designer plugin:
 * Designer provides a toolbar which is subject to a context change (to
 * "edit mode" context) when it is focused.
 * In order to prevent its actions from being disabled/hidden by that context
 * change, the actions are registered on the global context. In currentEditorChanged(),
 * the ones that are present in the global edit menu are set visible/invisible manually.
 * The designer context is currently used for Cut/Copy/Paste, etc. */

static inline QIcon designerIcon(const QString &iconName)
{
    // TODO: Create an icon...
    const QIcon icon = QIcon(iconName);
    if (icon.isNull())
        qWarning() << "Unable to locate " << iconName;
    return icon;
}

using namespace Core;
using namespace Designer::Constants;

namespace Designer {
namespace Internal {

// --------- DesignEditorW

DesignEditorW *DesignEditorW::m_self = 0;

DesignEditorW::DesignEditorW() :
    m_formeditor(0),
    m_ldesignerfactory(0),
    m_initStage(RegisterPlugins),
    m_actionGroupEditMode(0),
    m_actionPreview(0),
    m_actionGroupPreviewInStyle(0),
    m_previewInStyleMenu(0),
    m_actionAboutPlugins(0),
    m_shortcutMapper(new QSignalMapper(this)),
    m_context(0),
    m_modeWidget(0),
    m_editorWidget(0),
    m_designMode(0),
    m_editorToolBar(0),
    m_toolBar(0)
{
    if (Designer::Constants::Internal::debug)
        qDebug() << Q_FUNC_INFO;
    QTC_ASSERT(!m_self, return);
    m_self = this;

    qFill(m_designerSubWindows, m_designerSubWindows + Designer::Constants::DesignerSubWindowCount,
          static_cast<QWidget *>(0));

    m_ldesignerfactory = new LDesignerFactory;

    m_contexts.add(Designer::Constants::C_FORMEDITOR);

    setupActions();

    connect(EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(currentEditorChanged(Core::IEditor*)));
    connect(m_shortcutMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(updateShortcut(QObject*)));
}

DesignEditorW::~DesignEditorW()
{
    if (m_context)
        ICore::removeContextObject(m_context);
    if (m_initStage == FullyInitialized) {
        QSettings *s = ICore::settings();
        s->beginGroup(QLatin1String(settingsGroupC));
        m_editorWidget->saveSettings(s);
        s->endGroup();

        m_designMode->unregisterDesignWidget(m_modeWidget);
    }

    qDeleteAll(m_settingsPages);
    m_settingsPages.clear();
    delete m_ldesignerfactory;

    m_self = 0;
}

// Add an actioon to toggle the view state of a dock window
void DesignEditorW::addDockViewAction(ActionContainer *viewMenu,
                                      int index, const Context &context,
                                      const QString &title, const Id &id)
{
    if (const QDockWidget *dw = m_editorWidget->designerDockWidgets()[index]) {
        QAction *action = dw->toggleViewAction();
        action->setText(title);
        Command *cmd = addToolAction(action, context, id, viewMenu, QString());
        cmd->setAttribute(Command::CA_Hide);
    }
}

void DesignEditorW::setupViewActions()
{
    // Populate "View" menu of form editor menu
    ActionContainer *viewMenu = ActionManager::actionContainer(Core::Constants::M_WINDOW_VIEWS);
    QTC_ASSERT(viewMenu, return);

    //    addDockViewAction(viewMenu, WidgetBoxSubWindow, m_contexts,
    //                      tr("Widget box"), "DesignEditor.WidgetBox");

    //    addDockViewAction(viewMenu, ObjectInspectorSubWindow, m_contexts,
    //                      tr("Object Inspector"), "DesignEditor.ObjectInspector");

    //    addDockViewAction(viewMenu, PropertyEditorSubWindow, m_contexts,
    //                      tr("Property Editor"), "DesignEditor.PropertyEditor");

    //    addDockViewAction(viewMenu, SignalSlotEditorSubWindow, m_contexts,
    //                      tr("Signals && Slots Editor"), "DesignEditor.SignalsAndSlotsEditor");

    //    addDockViewAction(viewMenu, ActionEditorSubWindow, m_contexts,
    //                      tr("Action Editor"), "DesignEditor.ActionEditor");
    // Lock/Reset
    Command *cmd = addToolAction(m_editorWidget->menuSeparator1(), m_contexts,
                                 "DesignEditor.SeparatorLock", viewMenu);
    cmd->setAttribute(Command::CA_Hide);

    cmd = addToolAction(m_editorWidget->toggleLockedAction(), m_contexts,
                        "DesignEditor.Locked", viewMenu);
    cmd->setAttribute(Command::CA_Hide);

    cmd = addToolAction(m_editorWidget->menuSeparator2(), m_contexts,
                        "DesignEditor.SeparatorReset", viewMenu);
    cmd->setAttribute(Command::CA_Hide);

    cmd = addToolAction(m_editorWidget->resetLayoutAction(), m_contexts,
                        "DesignEditor.ResetToDefaultLayout", viewMenu);
    connect(m_editorWidget, SIGNAL(resetLayout()),
            m_editorWidget, SLOT(resetToDefaultLayout()));
    cmd->setAttribute(Command::CA_Hide);
}

void DesignEditorW::fullInit()
{
    QTC_ASSERT(m_initStage == RegisterPlugins, return);
    QTime *initTime = 0;
    if (Designer::Constants::Internal::debug) {
        initTime = new QTime;
        initTime->start();
    }

    initDesignerSubWindows();

    if (m_actionAboutPlugins)
        m_actionAboutPlugins->setEnabled(true);

    if (Designer::Constants::Internal::debug) {
        qDebug() << Q_FUNC_INFO << initTime->elapsed() << "ms";
        delete initTime;
    }

    connect(EditorManager::instance(), SIGNAL(editorsClosed(QList<Core::IEditor*>)),
            SLOT(closeDesignEditorsForXmlEditors(QList<Core::IEditor*>)));
    // Nest toolbar and editor widget
    m_editorWidget = new EditorWidget(this);
    QSettings *settings = ICore::settings();
    settings->beginGroup(QLatin1String(settingsGroupC));
    m_editorWidget->restoreSettings(settings);
    settings->endGroup();

    m_editorToolBar = createEditorToolBar();
    m_toolBar = EditorManager::createToolBar();
    m_toolBar->setToolbarCreationFlags(EditorToolBar::FlagsStandalone);
    // This activates/deactivates the navigation buttons to the left of the toolbar.
    m_toolBar->setNavigationVisible(false);
    m_toolBar->addCenterToolBar(m_editorToolBar);

    m_designMode = DesignMode::instance();
    m_modeWidget = new QWidget;
    m_modeWidget->setObjectName(QLatin1String("DesignerModeWidget"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_toolBar);
    // Avoid mode switch to 'Edit' mode when the application started by
    // 'Run' in 'Design' mode emits output.
    MiniSplitter *splitter = new MiniSplitter(Qt::Vertical);
    splitter->addWidget(m_editorWidget);
    QWidget *outputPane = new OutputPanePlaceHolder(m_designMode, splitter);
    outputPane->setObjectName(QLatin1String("DesignerOutputPanePlaceHolder"));
    splitter->addWidget(outputPane);
    layout->addWidget(splitter);
    m_modeWidget->setLayout(layout);

    Context designerContexts = m_contexts;
    designerContexts.add(Core::Constants::C_EDITORMANAGER);
    m_context = new DesignerContext(designerContexts, m_modeWidget, this);
    ICore::addContextObject(m_context);

    m_designMode->registerDesignWidget(m_modeWidget, QStringList(QLatin1String(DESIGNER_MIMETYPE)), m_contexts);

    setupViewActions();

    m_initStage = FullyInitialized;
}

void DesignEditorW::initDesignerSubWindows()
{
    qFill(m_designerSubWindows, m_designerSubWindows + Designer::Constants::DesignerSubWindowCount, static_cast<QWidget*>(0));

    DesignEditorPlugin* instance = DesignEditorPlugin::instance();
    m_designerSubWindows[DesignerToolBarWindow] = (QWidget *)(instance->designerToolBox());
    m_designerSubWindows[DesignerToolBarWindow]->setWindowTitle(tr("Process Nodes Toolbar"));
    m_designerSubWindows[DesignerToolBarWindow]->setObjectName(QLatin1String("ProcessNodesToolbar"));
    m_designerSubWindows[DesignerToolBarWindow]->setMinimumHeight(100);

    //m_designerSubWindows[DesignerSideToolBarWindow]  = (QWidget *)(instance->designerSideToolBar());
    //m_designerSubWindows[DesignerSideToolBarWindow]->setWindowTitle(tr("Actions Toolbar"));
    //m_designerSubWindows[DesignerSideToolBarWindow]->setObjectName(QLatin1String("ActionsToolbar"));
    //m_designerSubWindows[DesignerSideToolBarWindow]->setMinimumHeight(100);
}

QList<Core::IOptionsPage *> DesignEditorW::optionsPages() const
{
    return m_settingsPages;
}

IEngine *DesignEditorW::activeEngine() const
{
    return activeEditor().widgetHost->designWindow()->widget()->engine();
}

void DesignEditorW::ensureInitStage(InitializationStage s)
{
    if (Designer::Constants::Internal::debug)
        qDebug() << Q_FUNC_INFO << s;
    if (!m_self)
        m_self = new DesignEditorW;
    if (m_self->m_initStage >= s)
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_self->fullInit();
    QApplication::restoreOverrideCursor();
}

DesignEditorW *DesignEditorW::instance()
{
    ensureInitStage(FullyInitialized);
    return m_self;
}

void DesignEditorW::deleteInstance()
{
    if(m_self) {
        delete m_self;
        m_self = 0;
    }
}

void DesignEditorW::setupActions()
{
    //menus
    ActionContainer *medit = ActionManager::actionContainer(Core::Constants::M_EDIT);
    ActionContainer *mformtools = ActionManager::actionContainer(M_FORMEDITOR);

    // Init actions

    // Copy Action
    QIcon copyIcon = QIcon(QLatin1String(Designer::Constants::ICON_COPY));
    m_actionDesignerCopy = new QAction(copyIcon, tr("Copy"), this);
    // override copy
    bindShortcut(ActionManager::registerAction(m_actionDesignerCopy, Core::Constants::COPY, m_contexts),
                 m_actionDesignerCopy);

    // Cut Action
    QIcon cutIcon = QIcon(QLatin1String(Designer::Constants::ICON_CUT));
    m_actionDesignerCut = new QAction(cutIcon, tr("Cut"), this);
    // override cut
    bindShortcut(ActionManager::registerAction(m_actionDesignerCut, Core::Constants::CUT, m_contexts),
                 m_actionDesignerCut);

    // Paste Action
    QIcon pasteIcon = QIcon(QLatin1String(Designer::Constants::ICON_PASTE));
    m_actionDesignerPaste = new QAction(pasteIcon, tr("Paste"), this);
    // override paste
    bindShortcut(ActionManager::registerAction(m_actionDesignerPaste, Core::Constants::PASTE, m_contexts),
                 m_actionDesignerCopy);

    // Delete Action
    QIcon deleteIcon = QIcon(QLatin1String(Designer::Constants::ICON_DELETE));
    m_actionDesignerDelete = new QAction(deleteIcon, tr("Delete"), this);

    //'delete' action. Do not set a shortcut as Designer handles
    // the 'Delete' key by event filter. Setting a shortcut triggers
    // buggy behaviour on Mac (Pressing Delete in QLineEdit removing the widget).
    Command *command;
    command = ActionManager::registerAction(m_actionDesignerDelete,
                                            "DesignEditor.Edit.Delete",
                                            m_contexts);
    bindShortcut(command, m_actionDesignerDelete);
    command->setAttribute(Command::CA_Hide);
    medit->addAction(command, Core::Constants::G_EDIT_COPYPASTE);

    m_actionGroupEditMode = new QActionGroup(this);
    m_actionGroupEditMode->setExclusive(true);
    connect(m_actionGroupEditMode, SIGNAL(triggered(QAction*)), this,
            SLOT(activateEditMode(QAction*)));

    //medit->addSeparator(m_contexts, Core::Constants::G_EDIT_OTHER);

    //tool actions
    m_toolActionIds.push_back("DesignEditor.Delete");
    // This is a mac workaround, Qt::CTRL corresponds to the command button
    // on mac and Qt::Key_Meta
    // and variants to the Ctrl button on mac...
    QKeySequence seq(Qt::CTRL+Qt::Key_Backspace);
    QKeySequence seqWin(Qt::Key_Delete);
    const QString designEditorDelete = UseMacShortcuts ? seq.toString() : seqWin.toString();
    addToolAction(m_actionDesignerDelete, m_contexts,
                  m_toolActionIds.back(), mformtools, designEditorDelete);

    m_toolActionIds.push_back("DesignEditor.Copy");
    const QString designEditorCopy = UseMacShortcuts ? tr("Meta+C") : tr("Ctrl+C");
    addToolAction(m_actionDesignerCopy, m_contexts,
                  m_toolActionIds.back(), mformtools, designEditorCopy);

    m_toolActionIds.push_back("DesignEditor.Cut");
    const QString designEditorCut = UseMacShortcuts ? tr("Meta+X") : tr("Ctrl+X");
    addToolAction(m_actionDesignerCut, m_contexts,
                  m_toolActionIds.back(), mformtools, designEditorCut);

    m_toolActionIds.push_back("DesignEditor.Paste");
    const QString designEditorPaste = UseMacShortcuts ? tr("Meta+V") : tr("Ctrl+V");
    addToolAction(m_actionDesignerPaste, m_contexts,
                  m_toolActionIds.back(), mformtools, designEditorPaste);

    connect(m_actionDesignerDelete,SIGNAL(triggered()), this,SLOT(designerDelete()));
    connect(m_actionDesignerCopy,SIGNAL(triggered()),   this,SLOT(designerCopy()));
    connect(m_actionDesignerPaste,SIGNAL(triggered()),  this,SLOT(designerPaste()));
    connect(m_actionDesignerCut,SIGNAL(triggered()),    this,SLOT(designerCut()));

    QAction* DesignRunAction = DesignEditorPlugin::instance()->runAction();
    QAction* DesignPauseAction = DesignEditorPlugin::instance()->pauseAction();
    QAction* DesignStopAction= DesignEditorPlugin::instance()->stopRunAction();
    QAction* DesignZoomInAction = DesignEditorPlugin::instance()->zoominAction();
    QAction* DesignZoomOutAction = DesignEditorPlugin::instance()->zoomoutAction();
    QAction* ResetZoomingAction = DesignEditorPlugin::instance()->resetZoomingAction();
    QAction* FitToWindowAction = DesignEditorPlugin::instance()->fitToWindowAction();
    connect(DesignRunAction, SIGNAL(triggered()), this, SLOT(runDesign()));
    connect(DesignStopAction, SIGNAL(triggered()), this, SLOT(stopDesign()));
    connect(DesignPauseAction, SIGNAL(triggered()), this, SLOT(pauseDesign()));
    connect(DesignZoomInAction, SIGNAL(triggered()), this, SLOT(zoomInDesign()));
    connect(DesignZoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutDesign()));
    connect(ResetZoomingAction, SIGNAL(triggered()), this, SLOT(resetZooming()));
    connect(FitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));
}

// Creates the toolbar above the main widget area.
QToolBar *DesignEditorW::createEditorToolBar() const
{
    QToolBar *editorToolBar = new QToolBar;
    const QList<Id>::const_iterator cend = m_toolActionIds.constEnd();
    for (QList<Id>::const_iterator it = m_toolActionIds.constBegin(); it != cend; ++it) {
        Command *cmd = ActionManager::command(*it);
        QTC_ASSERT(cmd, continue);
        QAction *action = cmd->action();
        if (!action->icon().isNull()) // Simplify grid has no action yet
            editorToolBar->addAction(action);
    }
    const int size = editorToolBar->style()->pixelMetric(QStyle::PM_SmallIconSize);
    editorToolBar->setIconSize(QSize(size, size));
    editorToolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    return editorToolBar;
}

ActionContainer *DesignEditorW::createPreviewStyleMenu(QActionGroup *actionGroup)
{
    const QString menuId = QLatin1String(M_FORMEDITOR_PREVIEW);
    ActionContainer *menuPreviewStyle = ActionManager::createMenu(M_FORMEDITOR_PREVIEW);
    menuPreviewStyle->menu()->setTitle(tr("Preview in"));

    // The preview menu is a list of invisible actions for the embedded design
    // device profiles (integer data) followed by a separator and the styles
    // (string data). Make device profiles update their text and hide them
    // in the configuration dialog.
    const QList<QAction*> actions = actionGroup->actions();

    const QString deviceProfilePrefix = QLatin1String("DeviceProfile");
    const QChar dot = QLatin1Char('.');

    foreach (QAction* a, actions) {
        QString name = menuId;
        name += dot;
        const QVariant data = a->data();
        const bool isDeviceProfile = data.type() == QVariant::Int;
        if (isDeviceProfile) {
            name += deviceProfilePrefix;
            name += dot;
        }
        name += data.toString();
        Command *command = ActionManager::registerAction(a, Id::fromString(name), m_contexts);
        bindShortcut(command, a);
        if (isDeviceProfile) {
            command->setAttribute(Command::CA_UpdateText);
            command->setAttribute(Command::CA_NonConfigurable);
        }
        menuPreviewStyle->addAction(command);
    }
    return menuPreviewStyle;
}

void DesignEditorW::setPreviewMenuEnabled(bool e)
{
    m_actionPreview->setEnabled(e);
    m_previewInStyleMenu->setEnabled(e);
}

void DesignEditorW::saveSettings(QSettings *s)
{
    s->beginGroup(QLatin1String(settingsGroupC));
    m_editorWidget->saveSettings(s);
    s->endGroup();
}

void DesignEditorW::critical(const QString &errorMessage)
{
    QMessageBox::critical(ICore::mainWindow(), tr("Designer"),  errorMessage);
}

// Apply the command shortcut to the action and connects to the command's keySequenceChanged signal
void DesignEditorW::bindShortcut(Command *command, QAction *action)
{
    m_commandToDesignerAction.insert(command, action);
    connect(command, SIGNAL(keySequenceChanged()),
            m_shortcutMapper, SLOT(map()));
    m_shortcutMapper->setMapping(command, command);
    updateShortcut(command);
}

// Create an action to activate a designer tool
QAction *DesignEditorW::createEditModeAction(QActionGroup *ag,
                                             const Context &context,
                                             ActionContainer *medit,
                                             const QString &actionName,
                                             const Id &id,
                                             int toolNumber,
                                             const QString &iconName,
                                             const QString &keySequence)
{
    Q_UNUSED(medit);
    QAction *rc = new QAction(actionName, ag);
    rc->setCheckable(true);
    if (!iconName.isEmpty())
        rc->setIcon(designerIcon(iconName));
    Command *command = ActionManager::registerAction(rc, id, context);
    command->setAttribute(Core::Command::CA_Hide);
    if (!keySequence.isEmpty())
        command->setDefaultKeySequence(QKeySequence(keySequence));
    bindShortcut(command, rc);
    //medit->addAction(command, Core::Constants::G_EDIT_OTHER);
    rc->setData(toolNumber);
    ag->addAction(rc);
    return rc;
}

// Create a tool action
Command *DesignEditorW::addToolAction(QAction *a, const Context &context, const Id &id,
                                      ActionContainer *c1, const QString &keySequence,
                                      Core::Id groupId)
{
    Command *command = ActionManager::registerAction(a, id, context);
    if (!keySequence.isEmpty())
        command->setDefaultKeySequence(QKeySequence(keySequence));
    if (!a->isSeparator())
        bindShortcut(command, a);
    c1->addAction(command, groupId);
    return command;
}

EditorData DesignEditorW::createEditor(QWidget *parent)
{
    if (Designer::Constants::Internal::debug)
        qDebug() << "DesignEditorW::createEditor";
    // Create and associate designer and text editor.
    EditorData data;

    LDesigner *designer = (LDesigner *) m_ldesignerfactory->createEditor();
    m_formeditor = designer;

    QTC_ASSERT(designer, return data);

    data.widgetHost = new SharedTools::WidgetHost( /* parent */ 0, designer);
    DesignerXmlEditorWidget *xmlEditor = new DesignerXmlEditorWidget(designer, parent);
    TextEditor::TextEditorSettings::initializeEditor(xmlEditor);
    data.m_xmlEditor = xmlEditor->designerEditor();
    m_editorWidget->add(data);

    m_toolBar->addEditor(xmlEditor->editor());

    return data;
}

LDesigner *DesignEditorW::designerEditor() const
{
    return m_formeditor;
}

QWidget * const*DesignEditorW::designerSubWindows() const
{
    return m_designerSubWindows;
}

void DesignEditorW::updateShortcut(QObject *command)
{
    Command *c = qobject_cast<Command *>(command);
    if (!c)
        return;
    QAction *a = m_commandToDesignerAction.value(c);
    if (!a)
        return;
    a->setShortcut(c->action()->shortcut());
}

void DesignEditorW::currentEditorChanged(IEditor *editor)
{

    if (editor && editor->document()->id() == Constants::K_DESIGNER_XML_EDITOR_ID) {
        DesignWindowEditor *xmlEditor = qobject_cast<DesignWindowEditor *>(editor);
        QTC_ASSERT(xmlEditor, return);
        ensureInitStage(FullyInitialized);
        SharedTools::WidgetHost *fw = m_editorWidget->designWindowEditorForXmlEditor(xmlEditor);
        QTC_ASSERT(fw, return);
        m_editorWidget->setVisibleEditor(xmlEditor);
        //        m_ldesignerfactory->setActiveFormWindow(fw->designWindow());
    }
}

EditorData DesignEditorW::activeEditor() const
{
    if (m_editorWidget)
        return m_editorWidget->activeEditor();
    return EditorData();
}

void DesignEditorW::activateEditMode(int id)
{
    Q_UNUSED(id);
    //    if (const int count = m_ldesignerfactory->formWindowCount())
    //        for (int i = 0; i <  count; i++)
    //             m_ldesignerfactory->formWindow(i)->setCurrentTool(id);
}

void DesignEditorW::activateEditMode(QAction* a)
{
    activateEditMode(a->data().toInt());
}

void DesignEditorW::toolChanged(int t)
{
    typedef QList<QAction *> ActionList;
    if (const QAction *currentAction = m_actionGroupEditMode->checkedAction())
        if (currentAction->data().toInt() == t)
            return;
    const ActionList actions = m_actionGroupEditMode->actions();
    const ActionList::const_iterator cend = actions.constEnd();
    for (ActionList::const_iterator it = actions.constBegin(); it != cend; ++it)
        if ( (*it)->data().toInt() == t) {
            (*it)->setChecked(true);
            break;
        }
}

void DesignEditorW::closeDesignEditorsForXmlEditors(QList<IEditor*> editors)
{
    foreach (IEditor *editor, editors)
        m_editorWidget->removeDesignWindowEditor(editor);
}

void DesignEditorW::designerDelete()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->del();
    }
}

void DesignEditorW::designerCut()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->cut();
    }
}

void DesignEditorW::designerCopy()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->copy();
    }
}

void DesignEditorW::designerPaste()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->paste();
    }
}

void DesignEditorW::designerBringToFront()
{

}

void DesignEditorW::designerSendToBack()
{

}

void DesignEditorW::runDesign()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->runDesign();
    }
}

void DesignEditorW::stopDesign()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->stopDesign();
    }
}

void DesignEditorW::pauseDesign()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->pauseDesign();
    }
}

void DesignEditorW::resumeDesign()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->resumeDesign();
    }
}

void DesignEditorW::zoomInDesign()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->zoomIn();
    }
}

void DesignEditorW::zoomOutDesign()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->zoomOut();
    }
}

void DesignEditorW::resetZooming()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->zoomReset();
    }
}

void DesignEditorW::fitToWindow()
{
    EditorData data = activeEditor();
    if(data) {
        LDesignerWidget *widget = data.widgetHost->designWindow()->widget();
        widget->fitToWindow();
    }
}

} // namespace Internal
} // namespace Designer
