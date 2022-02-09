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
#ifndef MATLABTEMPALTEWIZARDPAGE_H
#define MATLABTEMPALTEWIZARDPAGE_H

#include <QWizardPage>

namespace Ui {
class MatlabTempalteWizardPage;
}

namespace MatlabTemplate {
namespace Internal {

class MatlabTempalteWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit MatlabTempalteWizardPage(QWidget *parent = 0);
    ~MatlabTempalteWizardPage();

    QString content() const;

    QString functionName() const;

private:
    Ui::MatlabTempalteWizardPage *ui;
};

}
}

#endif // MATLABTEMPALTEWIZARDPAGE_H
