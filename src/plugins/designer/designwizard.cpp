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
#include "designwizard.h"
#include "designwizarddialog.h"
#include "designerconstants.h"

#include <QStandardPaths>
#include <QDebug>

using namespace Designer;
using namespace Designer::Internal;

DesignWizard::DesignWizard()
{
    setFlags(WizardFlag::PlatformIndependent);
    setWizardKind(WizardKind::FileWizard);
}

QWizard *DesignWizard::createWizardDialog(QWidget *parent,
                                        const Core::WizardDialogParameters &wizardDialogParameters) const
{
    Q_UNUSED(wizardDialogParameters)

    DesignFileWizardDialog *wizardDialog = new DesignFileWizardDialog(parent);
    QString homeDirectory = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    wizardDialog->setPath(homeDirectory);
    return wizardDialog;
}

Core::GeneratedFiles DesignWizard::generateFiles(const QWizard *w,
                                               QString *errorMessage) const
{
    const DesignFileWizardDialog *wizard = qobject_cast<const DesignFileWizardDialog *>(w);
    const QString fileName = Core::BaseFileWizard::buildFileName(wizard->path(), wizard->fileName(),
                                                                 preferredSuffix(QLatin1String(Constants::DESIGNER_MIMETYPE)));
    const QString formTemplate = wizard->templateContents();
    if (formTemplate.isEmpty()) {
        *errorMessage = QLatin1String("Internal error: DesignWizard::generateFiles: empty template contents");
        return Core::GeneratedFiles();
    }

    Core::GeneratedFile file(fileName);
    file.setContents(formTemplate);
    file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);
    return Core::GeneratedFiles() << file;
}
