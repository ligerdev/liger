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
#include "matlabtemplatewizard.h"
#include "matlabtemplatedialog.h"
#include <matlabtemplate/matlabtemplateconstants.h>
#include <QFileDialog>

namespace MatlabTemplate {
namespace Internal {
MatlabTemplateWizard::MatlabTemplateWizard()
{
    setFlags(WizardFlag::PlatformIndependent);
    setWizardKind(WizardKind::FileWizard);
}

QWizard *MatlabTemplateWizard::createWizardDialog(QWidget *parent,
                                                  const Core::WizardDialogParameters &wizardDialogParameters) const
{
    MatlabTemplateDialog *wizardDialog = new MatlabTemplateDialog(parent);

    foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
             wizardDialog->addPage(p);

    return wizardDialog;
}

Core::GeneratedFiles MatlabTemplateWizard::generateFiles(const QWizard *w,
                                                         QString *errorMessage) const
{
    Core::GeneratedFiles files;

    const MatlabTemplateDialog *wizard = qobject_cast<const MatlabTemplateDialog *>(w);
    QString content = wizard->content();
    if(content.isEmpty()) {
        errorMessage->append("No content");
        return files;
    }

    QString dir = QFileDialog::getExistingDirectory(0, "Open Directory",
                                                         QDir::homePath());
    if(dir.isEmpty()) {
        errorMessage->append("Directory not selected.");
        return files;
    }

    QString fileName = dir + "/" + wizard->functionName() + Constants::EXTENSION;
    Core::GeneratedFile file(fileName);
    file.setContents(content);
    file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);
    files.push_back(file);

    return files;
}

}
}
