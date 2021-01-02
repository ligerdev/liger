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
#ifndef DESIGNER_EDITORWIDGET_H
#define DESIGNER_EDITORWIDGET_H

#include "designerconstants.h"
#include <ldesigner/ldesigner.h>

#include <utils/fancymainwindow.h>

namespace SharedTools { class WidgetHost; }
namespace Core { class IEditor; }
namespace Designer {
class DesignWindowEditor;

namespace Internal {
struct EditorData;
class DesignEditorStack;
class DesignEditorW;

// Design mode main view.
class EditorWidget : public Utils::FancyMainWindow
{
    Q_OBJECT

public:
    explicit EditorWidget(DesignEditorW *fe, QWidget *parent = 0);

    QDockWidget* const* designerDockWidgets() const;

    // Form editor stack API
    void add(const EditorData &d);
    void removeDesignWindowEditor(Core::IEditor *xmlEditor);
    bool setVisibleEditor(Core::IEditor *xmlEditor);
    SharedTools::WidgetHost *designWindowEditorForXmlEditor(const Core::IEditor *xmlEditor) const;
    SharedTools::WidgetHost *designWindowEditorForDesignWindow(const Designer::LDesigner *fw) const;

    EditorData activeEditor() const;

public slots:
    void resetToDefaultLayout();

private:
    DesignEditorStack *m_stack;
    QDockWidget *m_designerDockWidgets[Designer::Constants::DesignerSubWindowCount];
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNER_EDITORWIDGET_H
