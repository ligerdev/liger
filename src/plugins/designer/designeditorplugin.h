/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#ifndef DESIGNEDITORPLUGIN_H
#define DESIGNEDITORPLUGIN_H

#include <designer/designer_global.h>
#include <extensionsystem/iplugin.h>
#include <designer/designerconstants.h>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace Designer {

class NodeIDFactory;
class DesignerToolBox;

namespace Internal {
struct DesignEditorPluginPrivate;
class DesignEditorFactory;
class SettingsPageProvider;

class DESIGNER_EXPORT DesignEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "Designer.json")

public:
    DesignEditorPlugin();
    ~DesignEditorPlugin();

    static DesignEditorPlugin *instance();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

    NodeIDFactory* nodeIDFactory()     { return m_nodeIDFactory;   }
    DesignerToolBox* designerToolBox() { return m_designerToolbox; }

    QAction* runAction()     const;
    QAction* pauseAction()   const;
    QAction* stopRunAction() const;
    QAction* zoominAction()  const;
    QAction* zoomoutAction() const;
    QAction* resetZoomingAction() const;
    QAction* fitToWindowAction()  const;

    static QString currentFile();

public slots:
    void swapRunButtonToPause();
    void resetRunButton();

private:
    static DesignEditorPlugin* m_instance;
    void initializeTemplates();

    QAction*               m_actionSwitchSource;
    DesignEditorFactory*   m_designEditorFactory;
    SettingsPageProvider*  m_settingsPageProvider;
    NodeIDFactory*         m_nodeIDFactory;
    DesignerToolBox*       m_designerToolbox;

    DesignEditorPluginPrivate *d;
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNEDITORPLUGIN_H
