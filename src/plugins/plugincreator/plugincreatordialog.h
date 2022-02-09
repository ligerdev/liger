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
#ifndef PLUGINCREATORDIALOG_H
#define PLUGINCREATORDIALOG_H

#include <utils/wizard.h>
#include "plugincreatorwizardpage.h"

namespace Utils { class FileWizardPage; }

namespace PluginCreator {
namespace Internal {

class PluginCreatorDialog : public Utils::Wizard
{
    Q_OBJECT

public:
    explicit PluginCreatorDialog(QWidget *parent = 0);

    Param *parameters() const;

public slots:

private:
    PluginCreatorWizardPage *m_page;

    mutable QString m_templateContents;
};

} // namespace Internal
} // namespace PluginCreator

#endif // PLUGINCREATORDIALOG_H
