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
#include <qtigon/dialogs/qoperatordiag.h>
#include <qtigon/dialogs/qoperatordiagtabitem.h>
#include "ui_qoperatordiag.h"
#include <QDebug>
#include <QInputDialog>
#include <QMetaProperty>
#include <tigon/Operators/IOperator.h>
using Tigon::TObject;
using Tigon::TString;
using Tigon::TStringList;

QOperatorDiag::QOperatorDiag(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QOperatorDiag)
{
    ui->setupUi(this);
    ui->propertyName->setReadOnly(true);
    ui->propertyDes->setReadOnly(true);
    ui->propertyType->setReadOnly(true);
    ui->propertyValue->setReadOnly(true);
    ui->configBox->setEnabled(false);
    m_dataModel = new QStandardItemModel(this);
    ui->dataView->setModel(m_dataModel);
    connect(ui->dataView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &QOperatorDiag::upadtePropertyTable);
    connect(ui->confirmationButtonBox, &QDialogButtonBox::accepted,
            this, &QOperatorDiag::saveConfig);
    connect(ui->confirmationButtonBox, &QDialogButtonBox::rejected,
            this, &QOperatorDiag::close);
}

QOperatorDiag::~QOperatorDiag()
{
    delete ui;
}

void QOperatorDiag::setup(Tigon::Operators::IOperator *thisOp)
{
    m_op = thisOp;
    this->setWindowTitle(QString::fromStdString(thisOp->name()));
    ui->opDescription->setText(QString::fromStdString(thisOp->description()));

    clear();
    loadPropertiesFromOp();
}

void QOperatorDiag::setInputDisabled(bool b)
{
    ui->confirmationButtonBox->setDisabled(b);
    ui->modifyDataButton->setDisabled(b);
    ui->resetAllButton->setDisabled(b);
    ui->resetThisButton->setDisabled(b);
    // Disable all user defined tabs
    for(int i=1; i<ui->tabWidget->count(); ++i) {
        ui->tabWidget->widget(i)->setDisabled(b);
    }
}

void QOperatorDiag::addToolWidget(QOperatorDiagTabItem *toolWidget, const QString &name)
{
    int idx = ui->tabWidget->count();
    ui->tabWidget->insertTab(idx, toolWidget, name);
}

void QOperatorDiag::clear()
{
    ui->configBox->setEnabled(true);
    m_dataModel->clear();
    ui->propertyName->clear();
    ui->propertyDes->clear();
    ui->propertyType->clear();
    ui->propertyValue->clear();
}

void QOperatorDiag::upadtePropertyDisplayData()
{
    clear();
    loadPropertiesFromOp();
}

void QOperatorDiag::upadtePropertyTable(const QModelIndex &index)
{
    ui->configBox->setEnabled(true);
    ui->propertyName->clear();
    ui->propertyDes->clear();
    ui->propertyType->clear();
    ui->propertyValue->clear();
    int childIndex = index.row();
    QStandardItem* item =  m_dataModel->itemFromIndex(index);
    QVariant value = item->data();
    ui->propertyName->setText(QString::fromStdString(
                                  m_op->propertyNames()[childIndex]));
    ui->propertyDes->setText(QString::fromStdString(
                                 m_op->propertyDescriptions()[childIndex]));
    ui->propertyType->setText(value.typeName());
    ui->propertyValue->setPlainText(value.toString());
}

/*!
 * \brief Load the properties from the algorithm.
 */
void QOperatorDiag::updateOperatorConfig()
{
    QModelIndex currentIndex = ui->dataView->currentIndex();
    int index = currentIndex.row();
    if(index != -1) {
        QString newData = ui->propertyValue->toPlainText();
        TObject* object = m_op;
        object->setProperty(currentIndex.data().toString().toStdString(),
                            newData.toStdString());
    }
}

void QOperatorDiag::saveConfig()
{
    for(int i=0; i<m_dataModel->rowCount(); i++) {
        QModelIndex index = m_dataModel->index(i,0);
        QStandardItem* property = m_dataModel->itemFromIndex(index);
        TObject* object = m_op;
        qDebug() << property->text() << "<-->" << property->data();

        QString name = property->text();
        QVariant value = property->data();
        object->setProperty(name.toStdString(), value.toString().toStdString());
    }

    // deal with customized tabs
    for(int i=1; i<ui->tabWidget->count(); i++) {
        QOperatorDiagTabItem* item = qobject_cast<QOperatorDiagTabItem*>(ui->tabWidget->widget(i));
        item->save();
    }
    emit opDataChanged();
    close();
}

void QOperatorDiag::on_resetThisButton_clicked()
{
    QModelIndex currentIndex  = ui->dataView->currentIndex();
    TObject* object = m_op;
    TString value = object->propertyValue(currentIndex.data().toString().toStdString());
    ui->propertyValue->setPlainText(QString::fromStdString(value));
    QStandardItem* item = m_dataModel->itemFromIndex(currentIndex);
    item->setData(QVariant::fromValue(QString::fromStdString(value)));
}

/*!
 * \brief Discard all changes.
 */
void QOperatorDiag::on_resetAllButton_clicked()
{
    m_dataModel->clear();
    loadPropertiesFromOp();
    ui->propertyName->clear();
    ui->propertyDes->clear();
    ui->propertyType->clear();
    ui->propertyValue->clear();

    // deal with customized tabs
    for(int i=1; i<ui->tabWidget->count(); i++) {
        QOperatorDiagTabItem* item = qobject_cast<QOperatorDiagTabItem*>(ui->tabWidget->widget(i));
        item->reset();
    }
}

/*!
 * \brief Modify the value of the property
 */
void QOperatorDiag::on_modifyDataButton_clicked()
{
    QModelIndex currentIndex = ui->dataView->currentIndex();
    bool ok;
    QString oldData = ui->propertyValue->toPlainText();
    QString newData = QInputDialog::getText(this, tr("Change Property Value"),
                                            tr("New value:"), QLineEdit::Normal,
                                            oldData, &ok);
    if (ok && !newData.isEmpty() && newData != oldData) {
        ui->propertyValue->setPlainText(newData);
        QStandardItem* item =  m_dataModel->itemFromIndex(currentIndex);
        item->setData(QVariant::fromValue(newData));
    }
}

void QOperatorDiag::loadPropertiesFromOp()
{
    TObject* object = m_op;
    TStringList pnames = object->propertyNames();
    TStringList pdes = object->propertyDescriptions();
    for (int j=0; j<object->propertyCount(); j++) {
        TString name = pnames[j];
        // Store property name and value
        QStandardItem* property = new QStandardItem(QString::fromStdString(name));
        property->setToolTip(QString::fromStdString(pdes[j]));
        property->setEditable(false);
        TString value = object->propertyValue(name);
        property->setData(QString::fromStdString(value));
        m_dataModel->appendRow(property);
    }
}
