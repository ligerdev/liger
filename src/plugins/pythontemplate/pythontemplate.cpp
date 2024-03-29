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
#include "pythontemplate.h"
#include "pythontemplateconstants.h"
#include "pythontemplatewizard.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMenu>
#include <QCoreApplication>

#include <QtCore/QtPlugin>

using namespace PythonTemplate::Internal;

PythonTemplatePlugin::PythonTemplatePlugin()
{
    // Create your members
}

PythonTemplatePlugin::~PythonTemplatePlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members - Remove in reverse order of registration!
    // Note that objects that have been added with: addAutoReleasedObject()
    // do not have to be manually removed.
	
}

bool PythonTemplatePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    Core::IWizardFactory *m_wizard = new PythonTemplateWizard;
    m_wizard->setWizardKind(Core::IWizardFactory::FileWizard);
    m_wizard->setCategory(QLatin1String(Constants::WIZARD_CATEGORY));
    m_wizard->setDisplayCategory(QCoreApplication::translate("Designer",Constants::WIZARD_TR_CATEGORY));
    m_wizard->setDisplayName(tr(Constants::WIZARD_DISPLAY_NAME));
    m_wizard->setId(QLatin1String(Constants::WIZARD_ID));
    m_wizard->setDescription(tr(Constants::WIZARD_DESCRIPTIONS));
    addAutoReleasedObject(m_wizard);

    return true;
}

void PythonTemplatePlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
	
}

ExtensionSystem::IPlugin::ShutdownFlag PythonTemplatePlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(PythonTemplate)
