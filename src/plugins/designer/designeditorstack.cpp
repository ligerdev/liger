/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#include "designeditorstack.h"
#include "designwindoweditor.h"
#include "designeditorw.h"

#include <designer/ldesigner/widgethost.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/imode.h>

#include <utils/qtcassert.h>

#include <QDebug>
#include <QRect>

namespace Designer {
namespace Internal {

DesignEditorStack::DesignEditorStack(QWidget *parent) :
    QStackedWidget(parent)
{
    setObjectName(QLatin1String("DesignEditorStack"));
    connect(Core::ModeManager::instance(), SIGNAL(currentModeAboutToChange(Core::IMode*)),
            this, SLOT(modeAboutToChange(Core::IMode*)));
}

void DesignEditorStack::add(const EditorData &data)
{

    if (Designer::Constants::Internal::debug)
        qDebug() << "DesignEditorStack::add"  << data.m_xmlEditor << data.widgetHost;

    // m_designEditors is a list of EditorData, which essentially contains
    // an IEditor (m_xmlEditor) and a widgetHost (QScrollArea).
    m_designEditors.append(data);
    // adds the widgetHost (QScrollArea) to the widget stack: QStackedWidget.
    addWidget(data.widgetHost);
    // Editors are normally removed by listening to EditorManager::editorsClosed.
    // However, in the case opening a file fails, EditorManager just deletes the editor, which
    // is caught by the destroyed() signal.
    connect(data.m_xmlEditor, SIGNAL(destroyed(QObject*)),
            this, SLOT(removeDesignWindowEditor(QObject*)));

    if (Designer::Constants::Internal::debug)
        qDebug() << "DesignEditorStack::add" << data.widgetHost << m_designEditors.size();

    // Since we have 1 pixel splitters we enforce no frame
    // on the content widget
    if (QFrame *frame = qobject_cast<QFrame*>(data.widgetHost))
        frame->setFrameStyle(QFrame::NoFrame);
}

int DesignEditorStack::indexOfDesignWindow(const LDesigner *fw) const
{
    const int count = m_designEditors.size();
     for (int i = 0; i < count; ++i)
         if (m_designEditors[i].widgetHost->designWindow() == fw)
             return i;
     return -1;
}

int DesignEditorStack::indexOfDesignEditor(const QObject *xmlEditor) const
{
    const int count = m_designEditors.size();
    for (int i = 0; i < count; ++i)
        if (m_designEditors[i].m_xmlEditor == xmlEditor)
            return i;
    return -1;
}

EditorData DesignEditorStack::activeEditor() const
{
    // TODO: Should return the data from the active editor.
    // DONE: Now need to test that the correct data is returned.
    for(int i=0; i < m_designEditors.size(); i++) {
        EditorData cur = m_designEditors.at(i);
        bool res = (cur.widgetHost->designWindow()->isActive());
        if(res)
            return cur;
    }

    return EditorData();
}

SharedTools::WidgetHost *DesignEditorStack::designWindowEditorForDesignWindow(const LDesigner *fw) const
{
    const int i = indexOfDesignWindow(fw);
    return i != -1 ? m_designEditors[i].widgetHost : static_cast<SharedTools::WidgetHost *>(0);
}

void DesignEditorStack::removeDesignWindowEditor(QObject *xmlEditor)
{
    const int i = indexOfDesignEditor(xmlEditor);
    if (Designer::Constants::Internal::debug)
        qDebug() << "DesignEditorStack::removeDesignWindowEditor"  << xmlEditor << i << " of " << m_designEditors.size() ;
    if (i == -1) // Fail silently as this is invoked for all editors from EditorManager
        return;  // and editor deletion signal.

    removeWidget(m_designEditors[i].widgetHost);
    m_designEditors[i].widgetHost->deleteLater();
    m_designEditors.removeAt(i);
}

bool DesignEditorStack::setVisibleEditor(Core::IEditor *xmlEditor)
{
    if (Designer::Constants::Internal::debug)
        qDebug() << "DesignEditorStack::setVisibleEditor"  << xmlEditor;
    const int i = indexOfDesignEditor(xmlEditor);
    QTC_ASSERT(i != -1, return false);

    if (i != currentIndex())
        setCurrentIndex(i);
    return true;
}

SharedTools::WidgetHost *DesignEditorStack::designWindowEditorForXmlEditor(const Core::IEditor *xmlEditor) const
{
    const int i = indexOfDesignEditor(xmlEditor);
    return i != -1 ? m_designEditors.at(i).widgetHost : static_cast<SharedTools::WidgetHost *>(0);
}

void DesignEditorStack::modeAboutToChange(Core::IMode *m)
{
    if (Designer::Constants::Internal::debug && m)
        qDebug() << "DesignEditorStack::modeAboutToChange"  << m->id().toString();

    // Sync the editor when entering edit mode
    if (m && m->id() == Core::Constants::MODE_EDIT)
        foreach (const EditorData &data, m_designEditors)
            data.m_xmlEditor->syncXmlEditor();
}

} // Internal
} // Designer
