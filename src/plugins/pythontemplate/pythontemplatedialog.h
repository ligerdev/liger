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
#ifndef PYTHONTEMPLATEDIALOG_H
#define PYTHONTEMPLATEDIALOG_H

#include <utils/wizard.h>

namespace PythonTemplate {
namespace Internal {
class PythonTempalteWizardPage;

class PythonTemplateDialog : public Utils::Wizard
{
    Q_OBJECT

public:
    explicit PythonTemplateDialog(QWidget *parent = 0,
                                  Qt::WindowFlags flags = 0);
    QString content() const;
    QString functionName() const;

private:
    PythonTempalteWizardPage* m_page;
};

}
}

#endif // PYTHONTEMPLATEDIALOG_H
