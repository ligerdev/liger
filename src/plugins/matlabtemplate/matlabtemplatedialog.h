/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef MATLABTEMPLATEDIALOG_H
#define MATLABTEMPLATEDIALOG_H

#include <utils/wizard.h>

namespace MatlabTemplate {
namespace Internal {
class MatlabTempalteWizardPage;

class MatlabTemplateDialog : public Utils::Wizard
{
    Q_OBJECT

public:
    explicit MatlabTemplateDialog(QWidget *parent = 0,
                                  Qt::WindowFlags flags = 0);
    QString content() const;
    QString functionName() const;

private:
    MatlabTempalteWizardPage* m_page;
};

}
}

#endif // MATLABTEMPLATEDIALOG_H
