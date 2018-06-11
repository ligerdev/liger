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
#ifndef DESIGNEDITORW_H
#define DESIGNEDITORW_H

#include "designerconstants.h"
#include "designer_global.h"

#include <coreplugin/icontext.h>
#include <coreplugin/dialogs/ioptionspage.h>

#include <QMap>
#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QSignalMapper;
class QSettings;
class QToolBar;
QT_END_NAMESPACE

namespace Core {
class ActionManager;
class ActionContainer;
class Command;
class IEditor;
class Id;
class DesignMode;
class EditorToolBar;
}

namespace Designer {
class LDesigner;
class IEngine;

namespace Internal {
class LDesignerFactory;
struct EditorData;
class EditorWidget;
class SettingsPage;
class DesignerContext;

/** DesignEditorW is a singleton that stores the Designer CoreInterface and
  * performs centralized operations. The instance() function will return an
  * instance. However, it must be manually deleted when unloading the
  * plugin. Since fully initializing Designer at startup is expensive, the
  * class has an internal partial initialisation stage "RegisterPlugins"
  * which is there to register the Creator plugin objects
  * that must be present at startup (settings pages, actions).
  * The plugin uses this stage at first by calling ensureInitStage().
  * Requesting an editor via instance() will fully initialize the class.
  * This is based on the assumption that the Designer settings work with
  * no plugins loaded.
  *
  * The design editor shows a read-only XML editor in edit mode and Liger Designer
  * in Design mode. */
// TODO: Decouple the header from the implementation. (Create a private class
// to hold volatile types.
class DESIGNER_EXPORT DesignEditorW : public QObject
{
    Q_OBJECT
public:
    enum InitializationStage {
        // Register Creator plugins (settings pages, actions)
        RegisterPlugins,
        // Fully initialized for handling editor requests
        FullyInitialized
    };

    virtual ~DesignEditorW();

    // Create an instance and initialize up to stage s
    static void ensureInitStage(InitializationStage s);
    // Returns fully initialized instance
    static DesignEditorW *instance();
    // Deletes an existing instance if there is one.
    static void deleteInstance();

    EditorData createEditor(QWidget *parent = 0);

    LDesigner *designerEditor() const;// { return m_formeditor; }
    QWidget * const*designerSubWindows() const; // { return m_designerSubWindows; }

    EditorData activeEditor() const;
    QList<Core::IOptionsPage *> optionsPages() const;

    IEngine* activeEngine() const;

private slots:
    void activateEditMode(int id);
    void activateEditMode(QAction*);
    void currentEditorChanged(Core::IEditor *editor);
    void toolChanged(int);
    void setPreviewMenuEnabled(bool e);
    void updateShortcut(QObject *command);
    void closeDesignEditorsForXmlEditors(QList<Core::IEditor*> editors);

    // Designer actions
    void designerDelete();
    void designerCut();
    void designerCopy();
    void designerPaste();
    void designerBringToFront();
    void designerSendToBack();
    void runDesign();
    void stopDesign();
    void pauseDesign();
    void resumeDesign();
    void zoomInDesign();
    void zoomOutDesign();
    void resetZooming();
    void fitToWindow();

private:
    DesignEditorW();
    void fullInit();

    void saveSettings(QSettings *s);

    void initDesignerSubWindows();

    void setupActions();
    void setupViewActions();
    void addDockViewAction(Core::ActionContainer *viewMenu,
                           int index,
                           const Core::Context &context,
                           const QString &title, const Core::Id &id);

    Core::ActionContainer *createPreviewStyleMenu(QActionGroup *actionGroup);

    void critical(const QString &errorMessage);
    void bindShortcut(Core::Command *command, QAction *action);
    QAction *createEditModeAction(QActionGroup *ag,
                                         const Core::Context &context,
                                         Core::ActionContainer *medit,
                                         const QString &actionName,
                                         const Core::Id &id,
                                         int toolNumber,
                                         const QString &iconName = QString(),
                                         const QString &keySequence = QString());
    Core::Command *addToolAction(QAction *a,
                                 const Core::Context &context, const Core::Id &id,
                                 Core::ActionContainer *c1, const QString &keySequence = QString(),
                                 Core::Id groupId = Core::Id());
    QToolBar *createEditorToolBar() const;

    static DesignEditorW *m_self;

    LDesigner *m_formeditor;
    LDesignerFactory *m_ldesignerfactory;
    InitializationStage m_initStage;

    QWidget *m_designerSubWindows[Designer::Constants::DesignerSubWindowCount];

    QAction *m_lockAction;
    QAction *m_resetLayoutAction;

    QList<Core::IOptionsPage *> m_settingsPages;
    QActionGroup *m_actionGroupEditMode;

    QAction *m_actionDesignerDelete;
    QAction *m_actionDesignerCut;
    QAction *m_actionDesignerCopy;
    QAction *m_actionDesignerPaste;

    QAction *m_actionPreview;
    QActionGroup *m_actionGroupPreviewInStyle;
    QMenu *m_previewInStyleMenu;
    QAction *m_actionAboutPlugins;
    QSignalMapper *m_shortcutMapper;

    DesignerContext *m_context;
    Core::Context m_contexts;

    QList<Core::Id> m_toolActionIds;
    QWidget *m_modeWidget;
    EditorWidget *m_editorWidget;
    Core::DesignMode *m_designMode;

    // m_editorToolBar is the actual toolbar of the Designer.
    QWidget *m_editorToolBar;
    // m_toolBar is the underlying Top toolbar that comes from IEditor.
    Core::EditorToolBar *m_toolBar;

    QMap<Core::Command *, QAction *> m_commandToDesignerAction;
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNEDITORW_H
