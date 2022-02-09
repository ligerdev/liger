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

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

#include <QDialog>

namespace Ui {
class EmailNotifierConfigDialog;
}

class EmailNotifierConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmailNotifierConfigDialog(QWidget *parent = 0);
    ~EmailNotifierConfigDialog();

    void setSettings(QSettings* settings);
    void loadSettings();
    void saveSettings();

    QString smtp() const;
    int port() const;
    bool useSSl() const;
    bool useTCP() const;
    bool useTLS() const;
    QString userName() const;
    QString password() const;
    QString recipient() const;

    bool isPauseSelected() const;
    bool isFinishSelected() const;

private slots:
    void on_confirmButtonBox_accepted();
    void on_port_editingFinished();
    void on_hostComboBox_currentIndexChanged(int index);
    void on_hostLineEdit_editingFinished();
    void on_username_editingFinished();
    void on_recipient_editingFinished();
    void on_pausedCheckBox_toggled(bool checked);
    void on_finishedCheckBox_toggled(bool checked);

private:
    Ui::EmailNotifierConfigDialog *ui;
    int      m_port;
    bool     m_useTCP;
    bool     m_useSSl;
    bool     m_useTLS;
    bool     m_isPauseSelected;
    bool     m_isFinishSelected;
    QString  m_userName;
    QString  m_password;
    QString  m_recipient;
    QSettings* m_settings;
};
