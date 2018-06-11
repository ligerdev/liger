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
#include "pythontemplatedialog.h"
#include "pythontempaltewizardpage.h"
#include <QVariant>

namespace PythonTemplate {
namespace Internal {

PythonTemplateDialog::PythonTemplateDialog(QWidget *parent,
                                           Qt::WindowFlags flags)
    : Utils::Wizard(parent, flags)
    , m_page(new PythonTempalteWizardPage)
{
    addPage(m_page);
}

QString PythonTemplateDialog::content() const
{
    return m_page->content();
}

QString PythonTemplateDialog::functionName() const
{
    return field("functionName").toString();
}

}
}
