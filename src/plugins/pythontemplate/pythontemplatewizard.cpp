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
#include "pythontemplatewizard.h"
#include "pythontemplatedialog.h"
#include <pythontemplate/pythontemplateconstants.h>
#include <QFileDialog>

namespace PythonTemplate {
namespace Internal {
PythonTemplateWizard::PythonTemplateWizard()
{
    setFlags(WizardFlag::PlatformIndependent);
    setWizardKind(WizardKind::FileWizard);
}

QWizard *PythonTemplateWizard::createWizardDialog(QWidget *parent,
                                                  const Core::WizardDialogParameters &wizardDialogParameters) const
{
    PythonTemplateDialog *wizardDialog = new PythonTemplateDialog(parent);

    foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
             wizardDialog->addPage(p);

    return wizardDialog;
}

Core::GeneratedFiles PythonTemplateWizard::generateFiles(const QWizard *w,
                                                         QString *errorMessage) const
{
    Core::GeneratedFiles files;

    const PythonTemplateDialog *wizard = qobject_cast<const PythonTemplateDialog *>(w);
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
