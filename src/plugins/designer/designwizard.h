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
#ifndef DESIGNWIZARD_H
#define DESIGNWIZARD_H

#include <coreplugin/basefilewizard.h>

namespace Designer {
namespace Internal {

class DesignWizard : public Core::BaseFileWizard
{
    Q_OBJECT

public:
    DesignWizard();

private:
    QWizard *createWizardDialog(QWidget *parent,
                                const Core::WizardDialogParameters &wizardDialogParameters) const;

    Core::GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const;
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNWIZARD_H
