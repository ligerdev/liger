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
#include "designwizarddialog.h"

#include <coreplugin/basefilewizard.h>

#include <utils/filewizardpage.h>

#include <QDebug>
#include <QAbstractButton>

namespace Designer {
namespace Internal {

DesignFileWizardDialog::DesignFileWizardDialog(QWidget *parent)
  : Utils::Wizard(parent),
    m_filePage(new Utils::FileWizardPage)
{
    setPage(0, m_filePage);
    wizardProgress()->item(0)->setTitle(tr("Location"));
    connect(m_filePage, SIGNAL(activated()),
            button(QWizard::FinishButton), SLOT(animateClick()));
    m_templateContents = QString("<?xml version=\"1.0\"?>\n"
                                 "<LigerData/>");
}

QString DesignFileWizardDialog::path() const
{
    return m_filePage->path();
}

void DesignFileWizardDialog::setPath(const QString &path)
{
    m_filePage->setPath(path);
}

QString DesignFileWizardDialog::fileName() const
{
    return m_filePage->fileName();
}

QString DesignFileWizardDialog::templateContents() const
{
    return m_templateContents;
}

} // namespace Internal
} // namespace Designer
