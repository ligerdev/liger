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
#include "plugincreatordialog.h"


#include <coreplugin/basefilewizard.h>

#include <utils/filewizardpage.h>

#include <QDebug>
#include <QAbstractButton>

namespace PluginCreator {
namespace Internal {

PluginCreatorDialog::PluginCreatorDialog(QWidget *parent)
    : Utils::Wizard(parent),
      m_page(new PluginCreatorWizardPage)
  {
      setPage(0, m_page);
      wizardProgress()->item(0)->setTitle(tr("Information"));
      connect(m_page, SIGNAL(activated()),
              button(QWizard::FinishButton), SLOT(animateClick()));
  }

Param *PluginCreatorDialog::parameters() const
{
    return m_page->parameters();
}

} // namespace Internal
} // namespace PluginCreator
