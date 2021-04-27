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
#ifndef {% filter upper %}{{ plugin_name }}{% endfilter %}_H
#define {% filter upper %}{{ plugin_name }}{% endfilter %}_H

#include "{% filter lower %}{{ plugin_name }}{% endfilter %}_global.h"
#include "{% filter lower %}{{ plugin_name }}{% endfilter %}constants.h"
#include <extensionsystem/iplugin.h>

namespace {{plugin_name}} {
namespace Internal {

class  {{plugin_name}}Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "{{plugin_name}}.json")

public:
     {{plugin_name}}Plugin();
    ~{{plugin_name}}Plugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:

private:

};

} // namespace Internal
} // namespace {{plugin_name}}

#endif // {% filter upper %}{{ plugin_name }}{% endfilter %}_H
