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
#ifndef DESIGNERXMLEDITORWIDGET_H
#define DESIGNERXMLEDITORWIDGET_H

//#include "designwindowfile.h"

#include <texteditor/plaintexteditor.h>

#include <QSharedPointer>


namespace Designer {
class LDesigner;
class LDesignDocument;
class DesignWindowEditor;

namespace Internal {

/* A stub-like, read-only text editor which displays UI files as text. Could be used as a
  * read/write editor too, but due to lack of XML editor, highlighting and other such
  * functionality, editing is disabled.
  * Provides an informational title bar containing a button triggering a
  * switch to design mode.
  * Internally manages a DesignWindowEditor and uses the plain text
  * editable embedded in it.  */

class DesignerXmlEditorWidget : public TextEditor::PlainTextEditorWidget
{
    Q_OBJECT
public:
    explicit DesignerXmlEditorWidget(Designer::LDesigner *designer,
                               QWidget *parent = 0);

    DesignWindowEditor* designerEditor()   const;
    LDesignDocument*    designWindowFile() const;

protected:
    virtual TextEditor::BaseTextEditor* createEditor();

private:
    DesignWindowEditor *m_designerEditor;
};

} // Internal
} // Designer

#endif // DESIGNERXMLEDITORWIDGET_H
