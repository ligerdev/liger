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
#ifndef {{ UPPER_CLASS_PLUGIN_NAME }}_H
#define {{ UPPER_CLASS_PLUGIN_NAME }}_H

#include "{{ LOWER_CLASS_PLUGIN_NAME }}_global.h"
#include "{{ LOWER_CLASS_PLUGIN_NAME }}constants.h"
#include <extensionsystem/iplugin.h>

namespace {{ plugin_name }} {
namespace Internal {

class  {{ plugin_name }}Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "{{ plugin_name }}.json")

public:
     {{ plugin_name }}Plugin();
    ~{{ plugin_name }}Plugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:

private:

};

} // namespace Internal
} // namespace {{ plugin_name }}

#endif // {{ UPPER_CLASS_PLUGIN_NAME }}_H
