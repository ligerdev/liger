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
#ifndef EMAILNOTIFIER_H
#define EMAILNOTIFIER_H

#include <emailnotifier/emailnotifier_global.h>
#include <extensionsystem/iplugin.h>

#include <QAction>
#include <QSettings>
class EmailNotifierConfigDialog;

namespace EmailNotifier {
namespace Internal {

class  EmailNotifierPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "EmailNotifier.json")

public:
     EmailNotifierPlugin();
    ~EmailNotifierPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void configure();
    void sendNotificationsWhenFinished();
    void sendNotificationsWhenPaused();
    void saveSetting();
    void connectEngine();
    void disconnectEngine();

private:
    bool sendEmail();

    QAction*   m_config;
    EmailNotifierConfigDialog* m_diag;
    bool       m_isConfiged;
    QString    m_text;

};

} // namespace Internal
} // namespace EmailNotifier

#endif // EMAILNOTIFIER_H
