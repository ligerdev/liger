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
#pragma once
#include <qtigon/qtigon_global.h>
#include <qtigon/qtigonconstants.h>
#include <extensionsystem/iplugin.h>
#include <QObject>

namespace Designer {
class IEngine;
}

namespace QTigon {

namespace Internal {

class QTigonPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "QTigon.json")
    
public:
    QTigonPlugin();
    ~QTigonPlugin();
    
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();
    
private slots:
    void triggerAction();

};

} // namespace Internal
} // namespace QTigon

