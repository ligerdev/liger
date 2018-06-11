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
#include "designwindoweditor.h"
#include "designerconstants.h"
#include "designerxmleditorwidget.h"

#include <ldesigner/ldesigndocument.h>

#include <coreplugin/coreconstants.h>
#include <texteditor/basetextdocument.h>

#include <utils/qtcassert.h>

#include <QBuffer>
#include <QDebug>
#include <QFileInfo>

namespace Designer {

struct DesignWindowEditorPrivate
{
    Internal::DesignerXmlEditorWidget *m_widget;
};

DesignWindowEditor::DesignWindowEditor(Internal::DesignerXmlEditorWidget *editor) :
    TextEditor::PlainTextEditor(editor),
    d(new DesignWindowEditorPrivate)
{
    d->m_widget = editor;
    LDesignDocument *designDocument = d->m_widget->designWindowFile();
    setContext(Core::Context(Designer::Constants::K_DESIGNER_XML_EDITOR_ID,
                             Designer::Constants::C_DESIGNER_XML_EDITOR));

    // Revert to saved/load externally modified files.
    connect(designDocument, SIGNAL(reloadRequested(QString*,QString)),
            this, SLOT(slotOpen(QString*,QString)), Qt::DirectConnection);
}

DesignWindowEditor::~DesignWindowEditor()
{
    delete d;
}

void DesignWindowEditor::slotOpen(QString *errorString, const QString &fileName)
{
    open(errorString, fileName, fileName);
}

bool DesignWindowEditor::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
    LDesignDocument *designDocument = d->m_widget->designWindowFile();
    LDesigner *designer = designDocument->designWindow();

    QTC_ASSERT(designer, return false);

    if (fileName.isEmpty())
        return true;

    const QFileInfo fi(fileName);
    const QString absfileName = fi.absoluteFilePath();

    // Load the the file into: contents
    QString contents;
    if (d->m_widget->designWindowFile()->read(absfileName, &contents, errorString) != Utils::TextFileFormat::ReadSuccess)
        return false;

    document()->setFilePath(absfileName);

    // Why need this? Remove
    //d->m_widget->designWindowFile()->syncXmlFromDesignWindow();
    d->m_widget->designWindowFile()->setFilePath(absfileName);
    d->m_widget->designWindowFile()->setShouldAutoSave(false);
    d->m_widget->designWindowFile()->open(errorString, absfileName, realFileName);
    return true;
}

void DesignWindowEditor::syncXmlEditor()
{
    if (Designer::Constants::Internal::debug)
        qDebug() << "DesignWindowEditor::syncXmlEditor" << d->m_widget->designWindowFile()->filePath();
    d->m_widget->designWindowFile()->syncXmlFromDesignWindow();
}

QWidget *DesignWindowEditor::toolBar()
{
    return 0;
}

QString DesignWindowEditor::contents() const
{
    return d->m_widget->designWindowFile()->designWindowContents();
}

bool DesignWindowEditor::isDesignModePreferred() const
{
    return true;
}

} // namespace Designer

