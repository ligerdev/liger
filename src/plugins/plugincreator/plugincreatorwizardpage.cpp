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
#include "plugincreatorwizardpage.h"
#include "ui_plugincreatorwizardpage.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

using namespace PluginCreator::Internal;

PluginCreatorWizardPage::PluginCreatorWizardPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::PluginCreatorWizardPage)
{
    ui->setupUi(this);
}

PluginCreatorWizardPage::~PluginCreatorWizardPage()
{
    delete ui;
}

Param* PluginCreatorWizardPage::parameters() const
{
    Param* parameters = new Param;
    parameters->m_pluginName = ui->m_pluginName->text();
    parameters->m_pluginDescription = ui->m_pluginDescription->placeholderText();
    parameters->m_libDependencies = ui->m_libDependencies->text();
    parameters->m_pluginDependencies = ui->m_pluginDependencies->text();
    parameters->m_ligerRootPath = ui->m_ligerRootPath->text();

    return parameters;
}

void PluginCreatorWizardPage::on_find_clicked()
{
    QString ligerRootPath = QFileDialog::getExistingDirectory(this, tr("Find Liger Root Directory"),
                                                         QDir::homePath(),
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);

    ui->m_ligerRootPath->setText(ligerRootPath);
    checkLigerRootPath(ligerRootPath);
}

void PluginCreatorWizardPage::checkLigerRootPath(QString ligerRootPath)
{
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);

    QString srcFolderPath;
    // check if folder src, folder src/plugins and src/plugins/plugins.pro exist
    srcFolderPath = QDir(ligerRootPath).filePath("src");
    QFile ligerProFile(QDir(ligerRootPath).filePath("liger.pro"));
    QFile pluginsProFile(QDir(srcFolderPath).filePath("plugins/plugins.pro"));

    if ( !ligerProFile.exists() || !QDir(srcFolderPath).exists() ||
            !QDir( QDir(srcFolderPath).filePath("plugins") ).exists() ||
            !pluginsProFile.exists() )
    {
        ui->m_ligerRootPath->setText(QString::null);
        messageBox.critical(0,"Error","Something went wrong: \nwe couldn't regonize your given liger root directory :(.\nPlease try again.");
    }
}
