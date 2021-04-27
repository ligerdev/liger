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
#include "linkingvariablegrouppropertiesdialog.h"
#include "ui_linkingvariablegrouppropertiesdialog.h"

#include <QVariant>

using namespace QTigon;

LinkingVariableGroupPropertiesDialog::LinkingVariableGroupPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkingVariableGroupPropertiesDialog)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

LinkingVariableGroupPropertiesDialog::~LinkingVariableGroupPropertiesDialog()
{
    delete ui;
}

void LinkingVariableGroupPropertiesDialog::setup(const QStringList names,
                                                 const QVector<int> groupIds,
                                                 const QStringList &dataPath)
{
    int num = names.size();
    ui->tableWidget->setRowCount(num);
    for (int i=0; i<num; i++) {
        QTableWidgetItem *nameItem  = new QTableWidgetItem(names.at(i));
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 0, nameItem);
        QTableWidgetItem *idItem    = new QTableWidgetItem(QString::number(groupIds.at(i)));
        ui->tableWidget->setItem(i, 1, idItem);
        QTableWidgetItem *pathItem  = new QTableWidgetItem(dataPath.at(i));
        ui->tableWidget->setItem(i, 2, pathItem);
    }
}

void LinkingVariableGroupPropertiesDialog::on_buttonBox_rejected()
{

}

void LinkingVariableGroupPropertiesDialog::on_buttonBox_accepted()
{
    QVector<int> groupIds;
    QStringList pathes;
    for(int i=0; i<ui->tableWidget->rowCount(); i++) {
        groupIds.append(ui->tableWidget->item(i, 1)->data(Qt::DisplayRole).toInt());
        pathes.append(ui->tableWidget->item(i, 2)->text());
    }
    emit updateData(groupIds, pathes);
}
