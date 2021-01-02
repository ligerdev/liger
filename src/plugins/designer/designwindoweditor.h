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
#ifndef DESIGNWINDOWEDITOR_H
#define DESIGNWINDOWEDITOR_H

#include "designer_global.h"
#include <coreplugin/editormanager/ieditor.h>
#include <texteditor/plaintexteditor.h>

QT_BEGIN_NAMESPACE
class QDesignerFormWindowInterface;
QT_END_NAMESPACE

namespace TextEditor { class PlainTextEditor; }

namespace Designer {

namespace Internal { class DesignerXmlEditorWidget; }
struct DesignWindowEditorPrivate;

// IEditor that is used for the QDesignerFormWindowInterface
// It is a read-only PlainTextEditor that shows the XML of the form.
// DesignerXmlEditorWidget is the corresponding PlainTextEditorWidget,
// DesignWindowFile the corresponding BaseTextDocument.
// The content from the QDesignerFormWindowInterface is synced to the
// content of the XML editor.

class DESIGNER_EXPORT DesignWindowEditor : public TextEditor::PlainTextEditor
{
    Q_PROPERTY(QString contents READ contents)
    Q_OBJECT
public:
    explicit DesignWindowEditor(Internal::DesignerXmlEditorWidget *editor);
    virtual ~DesignWindowEditor();

    // IEditor
    virtual bool open(QString *errorString, const QString &fileName, const QString &realFileName);
    virtual QWidget *toolBar();
    virtual bool isDesignModePreferred() const;

    // For uic code model support
    QString contents() const;

public slots:
    void syncXmlEditor();

private slots:
    void slotOpen(QString *errorString, const QString &fileName);

private:
    DesignWindowEditorPrivate *d;
};

} // namespace Designer

#endif // DESIGNWINDOWEDITOR_H
