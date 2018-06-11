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
#include "qalgorithmdialog.h"
#include "ui_qalgorithmdialog.h"
#include <tigon/Algorithms/IAlgorithm.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Core/TObject.h>
#include <tigon/Core/TString.h>
using Tigon::TObject;
using Tigon::TString;
using Tigon::TStringList;

#include <QInputDialog>
#include <QModelIndex>
#include <QDebug>

QAlgorithmDialog::QAlgorithmDialog(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QAlgorithmDialog)
{
    ui->setupUi(this);
    this->setFrameStyle(QFrame::Panel | QFrame::Raised);
    ui->propertyName->setReadOnly(true);
    ui->propertyDes->setReadOnly(true);
    ui->propertyType->setReadOnly(true);
    ui->propertyValue->setReadOnly(true);
    ui->configBox->setEnabled(false);
    ui->dataView->header()->hide();

    m_dataModel = new QStandardItemModel(this);
    ui->dataView->setModel(m_dataModel);

    connect(ui->dataView->selectionModel(),&QItemSelectionModel::currentChanged,
            this, &QAlgorithmDialog::upadtePropertyDisplayData);
    connect(ui->dataView->selectionModel(),&QItemSelectionModel::currentChanged,
            ui->dataView, &QTreeView::expand);
    connect(ui->confirmationButtonBox, &QDialogButtonBox::accepted,
            this, &QAlgorithmDialog::saveConfig);
    connect(ui->confirmationButtonBox, &QDialogButtonBox::rejected,
            this, &QAlgorithmDialog::close);
}

QAlgorithmDialog::~QAlgorithmDialog()
{
    delete ui;
}

/*!
 * \brief Setup the widget
 * \param thisAlg the IAgorithm operator
 */
void QAlgorithmDialog::setup(Tigon::Algorithms::IAlgorithm* thisAlg)
{
    m_alg = thisAlg;
    this->setWindowTitle(QString::fromStdString(thisAlg->name()));
    ui->algDescription->setText(QString::fromStdString(thisAlg->description()));

    clear();
    loadPropertiesFromAlg();
}

void QAlgorithmDialog::clear()
{
    ui->propertyName->clear();
    ui->propertyDes->clear();
    ui->propertyType->clear();
    ui->propertyValue->clear();
    m_dataModel->clear();
}

void QAlgorithmDialog::setInputDisabled(bool b)
{
    ui->confirmationButtonBox->setDisabled(b);
    ui->resetAllButton->setDisabled(b);
    ui->modifyDataButton->setDisabled(b);
    ui->resetThisButton->setDisabled(b);
}

void QAlgorithmDialog::upadtePropertyDisplayData(QModelIndex index)
{
    ui->propertyName->clear();
    ui->propertyDes->clear();
    ui->propertyType->clear();
    ui->propertyValue->clear();
    int parentIndex = index.parent().row();
    if(parentIndex != -1) {
        ui->configBox->setEnabled(true);
        int childIndex = index.row();
        QStandardItem* item =  m_dataModel->itemFromIndex(index);
        QVariant value = item->data();
        ui->propertyName->setText(
                    QString::fromStdString(m_alg->operators()[parentIndex]->
                                           propertyNames()[childIndex]));
        ui->propertyDes->setText(
                    QString::fromStdString(m_alg->operators()[parentIndex]->
                                           propertyDescriptions()[childIndex]));
        ui->propertyType->setText(value.typeName());
        ui->propertyValue->setPlainText(variantToString(value));
    } else {
        ui->configBox->setEnabled(false);
    }
}

/*!
 * \brief Load the properties from the algorithm.
 */
void QAlgorithmDialog::updateOperatorConfig()
{ 
    QModelIndex currentIndex = ui->dataView->currentIndex();
    int index = currentIndex.parent().row();
    if(index != -1) {
        TString newData = ui->propertyValue->toPlainText().toStdString();
        TObject* object = m_alg->operators()[index];
        object->setProperty(currentIndex.data().toString().toStdString(),
                            newData);
    }
}

void QAlgorithmDialog::saveConfig()
{
    for(int i=0; i<m_dataModel->rowCount(); i++) {
        QModelIndex index = m_dataModel->index(i,0);
        QStandardItem* item = m_dataModel->itemFromIndex(index);
        TObject* object = m_alg->operators()[i];
        for(int j=0; j<item->rowCount(); j++) {
            QStandardItem* property = item->child(j);
            qDebug() << property->text() << "<-->" << property->data();

            object->setProperty(property->text().toStdString(),
                                property->data().toString().toStdString());
        }
    }
    emit algDataChanged();
    close();
}

/*!
 * \brief Discard the changes for the current property
 */
void QAlgorithmDialog::on_resetThisButton_clicked()
{
    QModelIndex currentIndex  = ui->dataView->currentIndex();
    int row = currentIndex.parent().row();
    if(row != -1) {
        TObject* object = m_alg->operators()[row];
        TString value = object->propertyValue(
                    currentIndex.data().toString().toStdString());
        ui->propertyValue->setPlainText(QString::fromStdString(value));
        QStandardItem* item = m_dataModel->itemFromIndex(currentIndex);
        item->setData(QVariant::fromValue(QString::fromStdString(value)));
    }
}

/*!
 * \brief Discard all changes.
 */
void QAlgorithmDialog::on_resetAllButton_clicked()
{
    clear();
    loadPropertiesFromAlg();
}

/*!
 * \brief Modify the value of the property
 */
void QAlgorithmDialog::on_modifyDataButton_clicked()
{
    QModelIndex currentIndex = ui->dataView->currentIndex();
    if(currentIndex.parent().row() == -1) {
        return;
    }
    bool ok;
    QString oldData = ui->propertyValue->toPlainText();
    QString newData = QInputDialog::getText(this, tr("Change Property Value"),
                                            tr("New value:"), QLineEdit::Normal,
                                            oldData, &ok);
    if (ok && !newData.isEmpty() && newData != oldData) {
        ui->propertyValue->setPlainText(newData);
        QStandardItem* item =  m_dataModel->itemFromIndex(currentIndex);
        item->setData(stringToVariant(newData, item->data().type()));
    }
}

void QAlgorithmDialog::loadPropertiesFromAlg()
{
    for(int i=0; i<m_alg->operators().size(); i++) {

        // Record Operators
        QStandardItem* tigonOp = new QStandardItem(
                    QString::fromStdString(m_alg->operators()[i]->name()));
        m_dataModel->appendRow(tigonOp);
        tigonOp->setToolTip(QString::fromStdString(
                                m_alg->operators()[i]->description()));
        tigonOp->setEditable(false);

        TObject* object = m_alg->operators()[i];
        TStringList pnames = object->propertyNames();
        TStringList pdes = object->propertyDescriptions();
        for (int j=0; j<object->propertyCount(); j++) {
            TString name = pnames[j];
            TString des = pdes[j];
            // Store property name and value
            QStandardItem* property = new QStandardItem(QString(name.c_str()));
            property->setToolTip(QString::fromStdString(des));
            property->setEditable(false);
            property->setData(QString::fromStdString(object->propertyValue(name)));
            tigonOp->appendRow(property);
        }
    }
}

QString QAlgorithmDialog::variantToString(QVariant variant) const
{
    if(variant.type() == QVariant::Type::StringList) {
        QStringList stringlist = variant.toStringList();
        QString tmp;
        for(int i=0; i<stringlist.size(); i++) {
            tmp.append(stringlist[i]);
            if(i != stringlist.size()-1) {
                tmp.append("; ");
            }
        }
        return tmp;
    } else {
        return variant.toString();
    }
}

QVariant QAlgorithmDialog::stringToVariant(QString str, QVariant::Type type) const
{
    QVariant variant;
    if(type == QVariant::Type::StringList) {
        QStringList tmp = str.split(";");
        QStringList result;
        for(int i=0; i<tmp.size(); i++) {
            tmp[i] = tmp[i].trimmed();
            if(!tmp[i].isEmpty()) {
                result.append(tmp[i]);
            }
        }
        result.removeDuplicates();
        variant = QVariant(result);
    } else {
        variant = QVariant(str);
        variant.convert(type);
    }
    return variant;
}
