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
#include "matlabtemplatedialog.h"
#include "matlabtempaltewizardpage.h"
#include <QVariant>

namespace MatlabTemplate {
namespace Internal {

MatlabTemplateDialog::MatlabTemplateDialog(QWidget *parent,
                                           Qt::WindowFlags flags)
    : Utils::Wizard(parent, flags)
    , m_page(new MatlabTempalteWizardPage)
{
    addPage(m_page);
}

QString MatlabTemplateDialog::content() const
{
    return m_page->content();
}

QString MatlabTemplateDialog::functionName() const
{
    return field("functionName").toString();
}

}
}
