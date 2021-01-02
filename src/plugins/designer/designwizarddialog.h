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
#ifndef DESIGNWIZARDDIALOG_H
#define DESIGNWIZARDDIALOG_H

#include <utils/wizard.h>

namespace Utils { class FileWizardPage; }

namespace Designer {
namespace Internal {

class DesignFileWizardDialog : public Utils::Wizard
{
    Q_OBJECT

public:
    explicit DesignFileWizardDialog(QWidget *parent = 0);

    QString path() const;
    QString fileName() const;
    QString templateContents() const;

public slots:
    void setPath(const QString &path);

private:
    Utils::FileWizardPage *m_filePage;
    mutable QString m_templateContents;
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNWIZARDDIALOG_H
