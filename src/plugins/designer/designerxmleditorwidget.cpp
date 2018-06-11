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
#include "designerxmleditorwidget.h"
#include "designwindoweditor.h"
#include "designerconstants.h"

#include <ldesigner/ldesigner.h>
#include <ldesigner/ldesigndocument.h>
#include <QDebug>

namespace Designer {
namespace Internal {

DesignerXmlEditorWidget::DesignerXmlEditorWidget(Designer::LDesigner *designer,
                                                 QWidget *parent) :
    TextEditor::PlainTextEditorWidget(new LDesignDocument(designer), parent),
    m_designerEditor(new DesignWindowEditor(this))
{

    /// EXPERIMENTAL
    designer->widget()->setDesignDocument(designWindowFile());
    ///////////////

    setReadOnly(true);
    configure(baseTextDocument()->mimeType());
}

TextEditor::BaseTextEditor *DesignerXmlEditorWidget::createEditor()
{
    if (Designer::Constants::Internal::debug)
        qDebug() << "DesignerXmlEditor::createEditableInterface()";
    return m_designerEditor;
}

DesignWindowEditor *DesignerXmlEditorWidget::designerEditor() const
{
    return m_designerEditor;
}

LDesignDocument *DesignerXmlEditorWidget::designWindowFile() const
{
    return qobject_cast<LDesignDocument *>(baseTextDocument());
}

//LDesignDocument *DesignerXmlEditorWidget::designDocument() const
//{
//    return m_designDocument;
//}

} // namespace Internal
} // namespace Designer

