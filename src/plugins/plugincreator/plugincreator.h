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
#ifndef PLUGINCREATOR_H
#define PLUGINCREATOR_H

#include "plugincreator_global.h"
#include "plugincreatorconstants.h"
#include <extensionsystem/iplugin.h>

namespace PluginCreator {
namespace Internal {

class  PluginCreatorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "PluginCreator.json")

public:
     PluginCreatorPlugin();
    ~PluginCreatorPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:


private:

};

} // namespace Internal
} // namespace PluginCreator

#endif // PLUGINCREATOR_H
