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
#ifndef DESIGNEDITORSTACK_H
#define DESIGNEDITORSTACK_H

#include "editordata.h"
#include <ldesigner/ldesigner.h>

#include <QStackedWidget>
#include <QList>


namespace Core {
    class IEditor;
    class IMode;
}

namespace Designer {
namespace Internal {

/* DesignEditorStack: Maintains a stack of Liger Designer windows embedded
 * into a scrollarea and their associated XML editors.
 * Takes care of updating the XML editor once design mode is left.
 * Also updates the maincontainer resize handles when the active form
 * window changes. */
class DESIGNER_EXPORT DesignEditorStack : public QStackedWidget
{
    Q_OBJECT

public:
    explicit DesignEditorStack(QWidget *parent = 0);

    void add(const EditorData &d);

    bool setVisibleEditor(Core::IEditor *xmlEditor);
    SharedTools::WidgetHost *designWindowEditorForXmlEditor(const Core::IEditor *xmlEditor) const;
    SharedTools::WidgetHost *designWindowEditorForDesignWindow(const LDesigner *fw) const;

    EditorData activeEditor() const;

public slots:
    void removeDesignWindowEditor(QObject *);

private slots:
    void modeAboutToChange(Core::IMode *);

private:
    inline int indexOfDesignWindow(const Designer::LDesigner *) const;
    inline int indexOfDesignEditor(const QObject *xmlEditor) const;

    QList<EditorData> m_designEditors;
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNEDITORSTACK_H
