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
#include "plugincreatorwizard.h"
#include "plugincreatordialog.h"
#include "plugincreatorconstants.h"

//#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QRegExp>

using namespace PluginCreator;
using namespace PluginCreator::Internal;

PluginCreatorWizard::PluginCreatorWizard()
{
    setFlags(WizardFlag::PlatformIndependent);
    setWizardKind(WizardKind::FileWizard);

    m_templatefilesList.append("t_wizard_customplugin.pro");
    m_templatefilesList.append("t_wizard_customplugin.pluginspec.in");
    m_templatefilesList.append("t_wizard_customplugin_dependencies.pri");
    m_templatefilesList.append("t_wizard_custompluginconstants.h");
    m_templatefilesList.append("t_wizard_customplugin_global.h");
    m_templatefilesList.append("t_wizard_customplugin.h");
    m_templatefilesList.append("t_wizard_customplugin.cpp");
}


QWizard *PluginCreatorWizard::createWizardDialog(QWidget *parent,
                                                 const Core::WizardDialogParameters &wizardDialogParameters) const
{
    PluginCreatorDialog *wizardDialog = new PluginCreatorDialog(parent);
   // QString homeDirectory = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
   // wizardDialog->setPath(homeDirectory);

    foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
             wizardDialog->addPage(p);

    return wizardDialog;
}

Core::GeneratedFiles PluginCreatorWizard::generateFiles(const QWizard *w, QString *errorMessage) const
{
    Core::GeneratedFiles files;

    const PluginCreatorDialog *wizard = qobject_cast<const PluginCreatorDialog *>(w);
    Param *parameters = wizard->parameters();

    // replacemap
    QMap<QString, QString> replacementMap = generateReplacementMap(parameters);

    QString templatePath = QDir(parameters->m_ligerRootPath).filePath(Constants::TEMPLATES_PATH);

    QString pluginPath = QDir(parameters->m_ligerRootPath).filePath("src/plugins");
    pluginPath = QDir(pluginPath).filePath(parameters->m_pluginName.toLower());

    QString content;

    // replace plugin.pro
    Core::GeneratedFile file( QDir(parameters->m_ligerRootPath).filePath(Constants::PLUGIN_PRO_PATH) );
    content = addToPluginPro(parameters);
    if (content.isEmpty())
    {
        *errorMessage = QLatin1String("Internal error: PluginCreatorWizard::generateFiles: plugins.pro not found or given plugin already exists");
        return Core::GeneratedFiles();
    }
    file.setContents(content);
    file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);
    files.push_back(file);

    for (int i = 0; i < m_templatefilesList.size(); i++)
    {
        // build file name
        QString tmpFileName = m_templatefilesList[i];
        const QString inFileName = QDir(templatePath).filePath(tmpFileName);

        tmpFileName.replace(0, 21, (parameters->m_pluginName.toLower()));
        const QString outFileName = QDir(pluginPath).filePath(tmpFileName);

        // read template content and generate file content
        content = readFile(inFileName, replacementMap);

        if (content.isEmpty())
        {
            *errorMessage = QLatin1String("Internal error: PluginCreatorWizard::generateFiles: empty file contents");
            return Core::GeneratedFiles();
        }

        Core::GeneratedFile file(outFileName);
        file.setContents(content);
        file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);
        files.push_back(file);
    }

    return files;
}

QString PluginCreatorWizard::readFile(const QString& fileName, const QMap<QString,QString>& replacementMap) const
{
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text))
            return QString::null;

    QString lines = file.readAll();

    QMap<QString,QString>::const_iterator itr = replacementMap.begin();
    QMap<QString,QString>::const_iterator end = replacementMap.end();

    while(itr != end)
    {
        lines.replace(itr.key(), itr.value());
        ++itr;
    }

    return lines;
}

QMap<QString,QString> PluginCreatorWizard::generateReplacementMap(const Param *parameters) const
{
    QMap<QString,QString> replacementMap;

    replacementMap["{{ UPPER_CLASS_PLUGIN_NAME }}"]   = parameters->m_pluginName.toUpper();
    replacementMap["{{ LOWER_CLASS_PLUGIN_NAME }}"]   = parameters->m_pluginName.toLower();
    replacementMap["{{ plugin_name }}"]               = parameters->m_pluginName;
    replacementMap["{{ plugin_description }}"]        = parameters->m_pluginDescription;

    QString tempString = parameters->m_libDependencies;
    replacementMap["{{ APPEND_LIB_DEPENDENCIES }}"]   = tempString.replace(",", " \\ \n   ");

    tempString = parameters->m_pluginDependencies;
    replacementMap["{{ APPEND_PLUGIN_DEPENDENCIES }}"]= tempString.replace(",", " \\ \n   ");

    return replacementMap;
}

QString PluginCreatorWizard::addToPluginPro(const Param *parameters) const
{
    QFile inFile( QDir(parameters->m_ligerRootPath).filePath(Constants::PLUGIN_PRO_PATH) );
    if (!inFile.open(QFile::ReadOnly | QFile::Text))
        return QString::null;

    QString lines = inFile.readAll();

    QRegExp rxCheckPluginExist("SUBDIRS\\s*\\+\\s*=\\s*" + parameters->m_pluginName.toLower());
    int  pos = 0;
    pos = rxCheckPluginExist.indexIn(lines,pos);
    if (pos == -1)
    {
        // find the index of the last SUBDIRS += ....
        // excluding  SUBDIR += matlabgate
        QRegExp rxFindInsertPos("(SUBDIRS)\\s*\\+\\s*=\\s* +(?!matlabgate)");
        pos = rxFindInsertPos.lastIndexIn(lines, pos);

        // find the position of the next new line symbol
        // where we register new plugin
        pos = lines.indexOf("\n", pos);

        lines.insert(pos, ("\nSUBDIRS += " + parameters->m_pluginName.toLower() + " \n"));
    }
    else
        return QString::null;

    return lines;
}
