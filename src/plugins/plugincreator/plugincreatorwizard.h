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
#ifndef PLUGINCREATORWIZARD_H
#define PLUGINCREATORWIZARD_H

#include <coreplugin/basefilewizard.h>
#include "plugincreatorwizardpage.h"

namespace PluginCreator {
namespace Internal {

class PluginCreatorWizard : public Core::BaseFileWizard
{
    Q_OBJECT

public:
    PluginCreatorWizard();

private:
    QWizard *createWizardDialog(QWidget *parent,
                                const Core::WizardDialogParameters &wizardDialogParameters) const;
    Core::GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const;

    /*
     * readFile
     * read in the templates and replace the replace the predefined patterns
     */
    QString readFile(const QString& fileName, const QMap<QString,QString>& replacementMap) const;

    /*
     * generateReplacementMap
     * replace the predefined patterns {{SOME_PATTERN_HERE}} in the templates
     * with the actual value of corresponding variables
     */
    QMap<QString, QString> generateReplacementMap(const Param* parameters) const;

    /*
     * addToPluginPro
     * update plugins.pro, registering new plugin
     */
    QString addToPluginPro(const Param* parameters) const;

    QStringList m_templatefilesList; // list of all template files
};

} // namespace Internal
} // namespace Designer

#endif // PLUGINCREATORWIZARD_H
