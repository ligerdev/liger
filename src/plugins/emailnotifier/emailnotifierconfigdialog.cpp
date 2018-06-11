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
#include <emailnotifier/emailnotifierconfigdialog.h>
#include <emailnotifier/emailnotifierconstants.h>
#include "ui_emailnotifierconfigdialog.h"

#include <QSettings>
using namespace EmailNotifier;

EmailNotifierConfigDialog::EmailNotifierConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EmailNotifierConfigDialog)
    , m_port(0)
    , m_useTCP(false)
    , m_useSSl(false)
    , m_useTLS(false)
    , m_isPauseSelected(false)
    , m_isFinishSelected(false)
    , m_settings(0)
{
    ui->setupUi(this);
    ui->noteTextBrowser->setReadOnly(true);
    connect(ui->confirmButtonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui->confirmButtonBox, SIGNAL(accepted()), this, SLOT(close()));
    connect(ui->confirmButtonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));
    connect(ui->confirmButtonBox, SIGNAL(rejected()), this, SLOT(close()));
}

EmailNotifierConfigDialog::~EmailNotifierConfigDialog()
{
    delete ui;
}

void EmailNotifierConfigDialog::setSettings(QSettings *settings)
{
    m_settings = settings;
}

void EmailNotifierConfigDialog::loadSettings()
{
    if(m_settings) {
        m_settings->beginGroup(Constants::EMAILNOTIFIER_SETTING_GROUP);
        ui->hostComboBox->setCurrentIndex(m_settings->value(Constants::SMTPCOMBO).toInt());
        ui->hostLineEdit->setText(m_settings->value(Constants::SMTP).toString());
        ui->port->setValue(m_settings->value(Constants::PORT).toInt());
        ui->protocols->setCurrentIndex(m_settings->value(Constants::PROTOCOL).toInt());
        ui->username->setText(m_settings->value(Constants::USERNAME).toString());
        ui->recipient->setText(m_settings->value(Constants::RECIPIENT).toString());
        m_settings->endGroup();
    }
}

void EmailNotifierConfigDialog::saveSettings()
{
    if(m_settings) {
        m_settings->beginGroup(Constants::EMAILNOTIFIER_SETTING_GROUP);
        m_settings->setValue(Constants::SMTPCOMBO, ui->hostComboBox->currentIndex());
        m_settings->setValue(Constants::SMTP, ui->hostLineEdit->text());
        m_settings->setValue(Constants::PORT, ui->port->value());
        m_settings->setValue(Constants::PROTOCOL, ui->protocols->currentIndex());
        m_settings->setValue(Constants::USERNAME, ui->username->text());
        m_settings->setValue(Constants::RECIPIENT, ui->recipient->text());
        m_settings->endGroup();
    }
}

void EmailNotifierConfigDialog::on_confirmButtonBox_accepted()
{
    m_port = ui->port->text().toInt();
    m_useTCP = ui->protocols->currentIndex() == 0;
    m_useSSl = ui->protocols->currentIndex() == 1;
    m_useTLS = ui->protocols->currentIndex() == 2;
    m_userName = ui->username->text();
    m_password = ui->password->text();
    m_recipient = ui->recipient->text();
    saveSettings();
}

QString EmailNotifierConfigDialog::recipient() const
{
    return m_recipient;
}

QString EmailNotifierConfigDialog::password() const
{
    return m_password;
}

QString EmailNotifierConfigDialog::userName() const
{
    return m_userName;
}

bool EmailNotifierConfigDialog::useSSl() const
{
    return m_useSSl;
}

bool EmailNotifierConfigDialog::useTCP() const
{
    return m_useTCP;
}

bool EmailNotifierConfigDialog::useTLS() const
{
    return m_useTLS;
}

int EmailNotifierConfigDialog::port() const
{
    return m_port;
}

QString EmailNotifierConfigDialog::smtp() const
{
    return ui->hostLineEdit->text();
}

void EmailNotifierConfigDialog::on_port_editingFinished()
{
    switch(ui->port->text().toInt()) {
    case 465:
        ui->protocols->setCurrentIndex(1);
        break;
    case 587:
        ui->protocols->setCurrentIndex(2);
        break;
    default:
        ui->protocols->setCurrentIndex(0);
        break;
    }
}

void EmailNotifierConfigDialog::on_hostComboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 1:
        ui->hostLineEdit->setText("smtp.gmail.com");
        ui->port->setValue(465);
        ui->protocols->setCurrentIndex(1);
        break;
    case 2:
        ui->hostLineEdit->setText("smtp.mail.yahoo.com");
        ui->port->setValue(465);
        ui->protocols->setCurrentIndex(1);
        break;
    case 3:
        ui->hostLineEdit->setText("smtp-mail.outlook.com");
        ui->port->setValue(587);
        ui->protocols->setCurrentIndex(2);
        break;
    default:
        ui->hostLineEdit->clear();
        ui->port->setValue(0);
        ui->protocols->setCurrentIndex(0);
        break;
    }
}

void EmailNotifierConfigDialog::on_hostLineEdit_editingFinished()
{
    ui->hostLineEdit->setText(ui->hostLineEdit->text().trimmed());
}

void EmailNotifierConfigDialog::on_username_editingFinished()
{
    ui->username->setText(ui->username->text().trimmed());
}

void EmailNotifierConfigDialog::on_recipient_editingFinished()
{
    ui->recipient->setText(ui->recipient->text().trimmed());
}

void EmailNotifierConfigDialog::on_pausedCheckBox_toggled(bool checked)
{
    m_isPauseSelected = checked;
}

void EmailNotifierConfigDialog::on_finishedCheckBox_toggled(bool checked)
{
    m_isFinishSelected = checked;
}

bool EmailNotifierConfigDialog::isFinishSelected() const
{
    return m_isFinishSelected;
}

bool EmailNotifierConfigDialog::isPauseSelected() const
{
    return m_isPauseSelected;
}
