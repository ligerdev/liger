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
#include <qtigon/dialogs/inputoutputcheckingform.h>
#include "ui_inputoutputcheckingform.h"

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <qtigon/dialogs/comboboxdelegate.h>

#include <QStandardItem>
#include <QInputDialog>
#include <QMouseEvent>

#include <QDebug>

using namespace Tigon::Representation;

namespace QTigon{
const QString renameAction("Rename");
const QString mergeAction("Merge");
const QString actionStatus("X");
const QString okStatus("OK");

InputOutputCheckingForm::InputOutputCheckingForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InputOutputCheckingForm)
    , m_numInputMerge(0)
    , m_numOutputMerge(0)
    , m_delegate(new ComboboxDelegate(this))
{
    ui->setupUi(this);
    ui->functNameLineEdit->setReadOnly(true);
    m_dataModel.setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    m_dataModel.setHorizontalHeaderItem(1, new QStandardItem(tr("Type")));
    m_dataModel.setHorizontalHeaderItem(2, new QStandardItem(tr("Action")));
    ui->tableView->setModel(&m_dataModel);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegate(m_delegate);
}

InputOutputCheckingForm::~InputOutputCheckingForm()
{
    delete ui;
    // no need to delete m_delegate as it is a child of this object
}

///\todo Delegate dropdown
void InputOutputCheckingForm::setFunction(IFunctionSPtr func,
                                          const QVector<int> &inputDuplications,
                                          const QVector<int> &outputDuplications)
{
    m_numInputMerge = 0;
    m_numOutputMerge = 0;
    m_dataModel.clear();
    m_dataModel.setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    m_dataModel.setHorizontalHeaderItem(1, new QStandardItem(tr("Type")));
    m_dataModel.setHorizontalHeaderItem(2, new QStandardItem(tr("Action")));
    ui->tableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);

    m_func = func;
    ui->functNameLineEdit->setText(QString::fromStdString(func->name()));

    int nInput = m_func->TP_nInputs();
    int nTotal = nInput + m_func->TP_nOutputs();

    QVector<QStringList> actions(nTotal);
    for(int i=0; i<nTotal; i++) {
        if(i<nInput) {
            actions[i] << renameAction << mergeAction;
        } else {
            actions[i] << renameAction;
        }
    }
    m_delegate->setNames(actions);
    m_delegate->setAcceptedIndex(2);

    m_inputDuplications = inputDuplications;
    m_outputDuplications = outputDuplications;

    Tigon::TVector<ElementProperties> iprts = m_func->inputPrpts();
    for(int i=0; i<iprts.size(); i++) {
        QList<QStandardItem*> list;
        list.append(new QStandardItem(QString::fromStdString(iprts[i].name())));
        list.append(new QStandardItem(tr("Input")));
        if(inputDuplications[i] != -1) {
            list.append(new QStandardItem(actionStatus));
        } else {
            list.append(new QStandardItem(okStatus));
        }
        m_dataModel.appendRow(list);
    }

    Tigon::TVector<ElementProperties> oprts = m_func->outputPrpts();
    for(int i=0; i<oprts.size(); i++) {
        QList<QStandardItem*> list;
        list.append(new QStandardItem(QString::fromStdString(oprts[i].name())));
        list.append(new QStandardItem(tr("Output")));
        if(outputDuplications[i] != -1) {
            list.append(new QStandardItem(actionStatus));
        } else {
            list.append(new QStandardItem(okStatus));
        }
        m_dataModel.appendRow(list);
    }
}

void QTigon::InputOutputCheckingForm::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(index.column() == 0) {
        QModelIndex idx = index.model()->index(index.row(), 2);
        if(idx.data().toString() == renameAction) {
            ui->tableView->edit(index);
        }
    }

    if(index.column() == 2) {
        ui->tableView->edit(index);
    }
}

void QTigon::InputOutputCheckingForm::on_confirmButtonBox_accepted()
{
    Tigon::TVector<ElementProperties> iprts =  m_func->inputPrpts();
    m_numInputMerge = 0;
    m_numOutputMerge = 0;

    ///\note rename: -1; no action: -1; merge: >=0; OK: -1
    for(int i=0; i<iprts.size(); i++) {
        if(m_inputDuplications[i] != -1) {
            QString action = m_dataModel.index(i, 2).data().toString();
            if(action == renameAction) {
                // Renamed
                QString newName = m_dataModel.index(i, 0).data().toString();
                iprts[i] = createElemPrpt(newName, iprts[i]);
                // renamed, no longer duplication
                m_inputDuplications[i] = -1;

            } else if(action == mergeAction) {
                ++m_numInputMerge;
            } else if(action == actionStatus){
                m_inputDuplications[i] = -1;
            }
        }
    }
    m_func->defineInputPrpts(iprts);

    Tigon::TVector<ElementProperties> oprts = m_func->outputPrpts();
    int nInput = iprts.size();
    for(int i=0; i<oprts.size(); i++) {
        if(m_outputDuplications[i] != -1) {
            QString action = m_dataModel.index(i+nInput, 2).data().toString();
            if(action == renameAction) {
                // Renamed
                QString newName = m_dataModel.index(i+nInput, 0).data().toString();
                oprts[i] = createElemPrpt(newName, oprts[i]);
                m_outputDuplications[i] = -1;

            } else if(action == mergeAction) {
                ++m_numOutputMerge;

            } else if(action == actionStatus) {
                m_outputDuplications[i] = -1;
            }
        }
    }
    m_func->defineOutputPrpts(oprts);

    emit duplicationHandled();
}

ElementProperties InputOutputCheckingForm::createElemPrpt(const QString &name,
                                                          ElementProperties oldElem)
{
    ElementPropertiesFactory* fct = ElementPropertiesFactory::instance();
    return fct->createElementProperties(oldElem.idx(), name.toStdString(),
                                        oldElem.description(), oldElem.type(),
                                        oldElem.optimizationType());
}

int InputOutputCheckingForm::numOutputMerge() const
{
    return m_numOutputMerge;
}

int InputOutputCheckingForm::numInputMerge() const
{
    return m_numInputMerge;
}

QVector<int> InputOutputCheckingForm::outputDuplications() const
{
    return m_outputDuplications;
}

QVector<int> InputOutputCheckingForm::inputDuplications() const
{
    return m_inputDuplications;
}

IFunctionSPtr InputOutputCheckingForm::function() const
{
    return m_func;
}

void InputOutputCheckingForm::on_confirmButtonBox_rejected()
{
    m_func.reset();
}

void InputOutputCheckingForm::on_mergeAllPushButton_clicked()
{
    int nrow = ui->tableView->model()->rowCount();
    for(int i=0; i<nrow; i++) {
        QString action = m_dataModel.index(i, 2).data().toString();
        if(action == actionStatus) {
            m_dataModel.setData(m_dataModel.index(i, 2), mergeAction);
        }
    }
}

void InputOutputCheckingForm::on_renameAllPushButton_clicked()
{
    int nrow = ui->tableView->model()->rowCount();
    for(int i=0; i<nrow; i++) {
        QString action = m_dataModel.index(i, 2).data().toString();
        if(action == actionStatus) {
            m_dataModel.setData(m_dataModel.index(i, 2), renameAction);
        }
    }
}
}
