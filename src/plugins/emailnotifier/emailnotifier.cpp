/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <emailnotifier/emailnotifier.h>
#include <emailnotifier/emailnotifierconfigdialog.h>
#include <emailnotifier/emailnotifierconstants.h>

#include <emailnotifier/smtpemail/SmtpMime>

#include <designer/designeditorplugin.h>
#include <designer/designeditorw.h>
#include <designer/iengine/iengine.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>

#include <QMenu>
#include <QtConcurrent>

using namespace EmailNotifier::Internal;
using namespace Designer::Internal;
using namespace Core;

EmailNotifierPlugin::EmailNotifierPlugin()
    : m_config(0)
    , m_diag(new EmailNotifierConfigDialog)
    , m_isConfiged(false)
{
}

EmailNotifierPlugin::~EmailNotifierPlugin()
{
    delete m_diag;
}

bool EmailNotifierPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    Core::Context C_LIGER_NOTIFIER(Core::Constants::C_GLOBAL);

    ActionContainer *mtools = ActionManager::actionContainer(Core::Constants::M_TOOLS);
    ActionContainer *memail = ActionManager::createMenu(Constants::MENU_ID);
    memail->menu()->setTitle(tr("Email Notifier"));
    mtools->addMenu(memail);

    Command* cmd_confg = ActionManager::registerAction(new QAction(this),
                                                      Constants::ACTION_CONFIG,
                                                      C_LIGER_NOTIFIER);
    cmd_confg->action()->setText(tr("&Configure"));
    memail->addAction(cmd_confg);
    connect(cmd_confg->action(), SIGNAL(triggered(bool)), this, SLOT(configure()));
    m_config = cmd_confg->action();

    connect(m_diag, SIGNAL(accepted()), this, SLOT(saveSetting()));

    m_diag->setSettings(Core::ICore::settings());
    return true;
}

void EmailNotifierPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
    DesignEditorPlugin* designer = DesignEditorPlugin::instance();
    connect(designer->runAction(), SIGNAL(triggered(bool)),
            this, SLOT(connectEngine()));
    connect(designer->stopRunAction(), SIGNAL(triggered(bool)),
            this, SLOT(disconnectEngine()));
}

ExtensionSystem::IPlugin::ShutdownFlag EmailNotifierPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void EmailNotifierPlugin::configure()
{
    m_diag->loadSettings();
    m_diag->show();
    m_diag->raise();
}

void EmailNotifierPlugin::sendNotificationsWhenFinished()
{
    if(!m_diag->isFinishSelected()) {
        return;
    }
    qDebug() << "Send notifications - finished";
    Designer::IEngine* engine = DesignEditorW::instance()->activeEngine();
    disconnect(engine, SIGNAL(engineFinished()), this, SLOT(sendNotificationsWhenFinished()));

    // First we need to create an SmtpClient object
    if(!m_isConfiged) {
        return;
    }

    m_text = "Hi,\nYour workflow has finished.\n";
    QtConcurrent::run(this, &EmailNotifierPlugin::sendEmail);
}

void EmailNotifierPlugin::sendNotificationsWhenPaused()
{
    if(!m_diag->isPauseSelected()) {
        return;
    }
    qDebug() << "Send notifications - paused";
    Designer::IEngine* engine = DesignEditorW::instance()->activeEngine();
    disconnect(engine, SIGNAL(enginePaused()), this, SLOT(sendNotificationsWhenPaused()));

    // First we need to create an SmtpClient object
    if(!m_isConfiged) {
        return;
    }

    m_text = "Hi,\nYour workflow has been paused.\n";
    QtConcurrent::run(this, &EmailNotifierPlugin::sendEmail);
}

void EmailNotifierPlugin::saveSetting()
{
    m_isConfiged = true;
}

void EmailNotifierPlugin::connectEngine()
{
    qDebug() << "Connect engine to notifier";
    Designer::IEngine* engine = DesignEditorW::instance()->activeEngine();
    connect(engine, SIGNAL(engineFinished()), this, SLOT(sendNotificationsWhenFinished()));
    connect(engine, SIGNAL(enginePaused()), this, SLOT(sendNotificationsWhenPaused()));
}

void EmailNotifierPlugin::disconnectEngine()
{
    qDebug() << "Disconnect engine from notifier";
    Designer::IEngine* engine = DesignEditorW::instance()->activeEngine();
    disconnect(engine, SIGNAL(engineFinished()), this, SLOT(sendNotificationsWhenFinished()));
    disconnect(engine, SIGNAL(enginePaused()), this, SLOT(sendNotificationsWhenPaused()));
}

bool EmailNotifierPlugin::sendEmail()
{
    SmtpClient::ConnectionType type = SmtpClient::TcpConnection;
    if(m_diag->useSSl()) {
        type = SmtpClient::SslConnection;
    } else if(m_diag->useTLS()) {
        type = SmtpClient::TlsConnection;
    }

    qDebug() << m_diag->smtp() << m_diag->port() << type;
    SmtpClient smtp(m_diag->smtp(), m_diag->port(), type);

    // Set the username (your email address) and password for smtp authentification.
    smtp.setUser(m_diag->userName());
    smtp.setPassword(m_diag->password());

    // Create a MimeMessage object. This is the email.
    MimeMessage message;

    EmailAddress sender(m_diag->userName(), "Liger");
    message.setSender(&sender);

    EmailAddress to(m_diag->recipient(), "Liger User");
    message.addRecipient(&to);

    message.setSubject("Liger Notification");

    // Add text to the email.
    MimeText text;
    text.setText(m_text);
    message.addPart(&text);

    // Send the email
    if (!smtp.connectToHost()) {
        qDebug() << "Failed to connect to host!" << endl;
        return false;
    }

    if (!smtp.login()) {
        qDebug() << "Failed to login!" << endl;
        return false;
    }

    if (!smtp.sendMail(message)) {
        qDebug() << "Failed to send mail!" << endl;
        return false;
    }

    smtp.quit();
    return true;
}

Q_EXPORT_PLUGIN(EmailNotifier)
