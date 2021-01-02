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
#include "editorwidget.h"
#include "designeditorw.h"
#include "designeditorstack.h"

#include <coreplugin/editormanager/ieditor.h>

#include <QDockWidget>
#include <QAbstractItemView>

using namespace Designer::Constants;

namespace Designer {
namespace Internal {

// ---------- EditorWidget

EditorWidget::EditorWidget(DesignEditorW *few, QWidget *parent) :
    Utils::FancyMainWindow(parent),
    m_stack(new DesignEditorStack)
{
    setObjectName(QLatin1String("EditorWidget"));
    setCentralWidget(m_stack);

//    setDocumentMode(true);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    QWidget * const*subs = few->designerSubWindows();
    for (int i = 0; i < DesignerSubWindowCount; i++) {
        QWidget *subWindow = subs[i];
        subWindow->setWindowTitle(subs[i]->windowTitle());
        m_designerDockWidgets[i] = addDockForWidget(subWindow);

        // Since we have 1-pixel splitters, we generally want to remove
        // frames around item views. So we apply this hack for now.
        QList<QAbstractItemView*> frames = subWindow->findChildren<QAbstractItemView*>();
        for (int i = 0 ; i< frames.count(); ++i)
            frames[i]->setFrameStyle(QFrame::NoFrame);

    }
    resetToDefaultLayout();
}

void EditorWidget::resetToDefaultLayout()
{
    setTrackingEnabled(false);
    QList<QDockWidget *> dockWidgetList = dockWidgets();
    foreach (QDockWidget *dockWidget, dockWidgetList) {
        dockWidget->setFloating(false);
        removeDockWidget(dockWidget);
    }

    addDockWidget(Qt::LeftDockWidgetArea,m_designerDockWidgets[DesignerToolBarWindow]);
    //addDockWidget(Qt::RightDockWidgetArea, m_designerDockWidgets[DesignerSideToolBarWindow]);

//    tabifyDockWidget(m_designerDockWidgets[ActionEditorSubWindow],
//                     m_designerDockWidgets[SignalSlotEditorSubWindow]);

    foreach (QDockWidget *dockWidget, dockWidgetList)
        dockWidget->show();

    setTrackingEnabled(true);
}

QDockWidget* const* EditorWidget::designerDockWidgets() const
{
    return m_designerDockWidgets;
}

void EditorWidget::add(const EditorData &d)
{
    m_stack->add(d);
}

void EditorWidget::removeDesignWindowEditor(Core::IEditor *xmlEditor)
{
    m_stack->removeDesignWindowEditor(xmlEditor);
}

bool EditorWidget::setVisibleEditor(Core::IEditor *xmlEditor)
{
    return m_stack->setVisibleEditor(xmlEditor);
}

SharedTools::WidgetHost *EditorWidget::designWindowEditorForXmlEditor(const Core::IEditor *xmlEditor) const
{
    return m_stack->designWindowEditorForXmlEditor(xmlEditor);
}

EditorData EditorWidget::activeEditor() const
{
    return m_stack->activeEditor();
}

SharedTools::WidgetHost *EditorWidget::designWindowEditorForDesignWindow(const LDesigner *fw) const
{
    return m_stack->designWindowEditorForDesignWindow(fw);
}

} // namespace Internal
} // namespace Designer
