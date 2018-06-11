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
#ifndef PLUGINCREATORWIZARDPAGE_H
#define PLUGINCREATORWIZARDPAGE_H

#include <QWizardPage>

namespace Ui {
class PluginCreatorWizardPage;
}

namespace PluginCreator {
namespace Internal {

// Struct that contains all variables passed
// from the UI
struct Param
{
    QString m_pluginName;
    QString m_pluginDescription;
    QString m_libDependencies;
    QString m_pluginDependencies;
    QString m_ligerRootPath;
};

class PluginCreatorWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit PluginCreatorWizardPage(QWidget *parent = 0);
    ~PluginCreatorWizardPage();

    Param* parameters() const;

private slots:
    void on_find_clicked();

private:
    void checkLigerRootPath(QString ligerRootPath); // check if the user provided path is valid
    Ui::PluginCreatorWizardPage *ui;
};

} // namespace Internal
} // namespace Designer

#endif // PLUGINCREATORWIZARDPAGE_H
