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
#ifndef LINKINGVARIABLEGROUPPROPERTIESDIALOG_H
#define LINKINGVARIABLEGROUPPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class LinkingVariableGroupPropertiesDialog;
}

namespace QTigon {
class LinkingVariableGroupPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LinkingVariableGroupPropertiesDialog(QWidget *parent = 0);
    ~LinkingVariableGroupPropertiesDialog();

    void setup(const QStringList names,
               const QVector<int> groupIds,
               const QStringList &dataPath);

signals:
    void updateData(const QVector<int> &, const QStringList &);

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

private:
    Ui::LinkingVariableGroupPropertiesDialog *ui;
};
}

#endif // LINKINGVARIABLEGROUPPROPERTIESDIALOG_H
