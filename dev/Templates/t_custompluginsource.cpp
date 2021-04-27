/****************************************************************************
**
** Copyright (C) 2012-2016 The University of Sheffield (www.sheffield.ac.uk)
** Copyright (C) 2012-2016 Ioannis Giagkiozis (www.sheffield.ac.uk)
**
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
#include "{% filter lower %}{{ plugin_name }}{% endfilter %}.h"
#include "{% filter lower %}{{ plugin_name }}{% endfilter %}constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMenu>

#include <QtCore/QtPlugin>

using namespace {{ plugin_name }}::Internal;

{{plugin_name}}Plugin::{{plugin_name}}Plugin()
{
    // Create your members
}

{{plugin_name}}Plugin::~{{plugin_name}}Plugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members - Remove in reverse order of registration!
    // Note that objects that have been added with: addAutoReleasedObject()
    // do not have to be manually removed.
	
}

bool {{plugin_name}}Plugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    return true;
}

void {{plugin_name}}Plugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
	
}

ExtensionSystem::IPlugin::ShutdownFlag {{plugin_name}}Plugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN({{ plugin_name }})
