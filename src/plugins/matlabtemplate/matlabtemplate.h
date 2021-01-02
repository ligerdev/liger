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
#ifndef MATLABTEMPLATE_H
#define MATLABTEMPLATE_H

#include "matlabtemplate_global.h"
#include "matlabtemplateconstants.h"
#include <extensionsystem/iplugin.h>

namespace MatlabTemplate {
namespace Internal {

class  MatlabTemplatePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "MatlabTemplate.json")

public:
     MatlabTemplatePlugin();
    ~MatlabTemplatePlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:

private:

};

} // namespace Internal
} // namespace MatlabTemplate

#endif // MATLABTEMPLATE_H
