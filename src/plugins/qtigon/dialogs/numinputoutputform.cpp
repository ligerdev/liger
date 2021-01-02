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
#include "numinputoutputform.h"
#include "ui_numinputoutputform.h"

#include <QMessageBox>
#include <QString>
using namespace QTigon;

NumInputOutputForm::NumInputOutputForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NumInputOutputForm)
{
    ui->setupUi(this);
}

NumInputOutputForm::~NumInputOutputForm()
{
    delete ui;
}

void NumInputOutputForm::on_buttonBox_accepted()
{
    int ninput = ui->ninputs->text().toInt();
    int noutput = ui->noutputs->text().toInt();
    if(ninput <= noutput) {
        ui->ninputs->clear();
        ui->noutputs->clear();
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Number of inputs must be greater than number of outputs"),
                             QMessageBox::Ok);
        this->show();
    } else {
        emit updateNumInputOutput(ninput, noutput);
    }
}

void NumInputOutputForm::resetForm()
{
    ui->ninputs->clear();
    setNumInputsReadOnly(false);
    ui->noutputs->clear();
    setNumOutputsReadOnly(false);
}

void NumInputOutputForm::setNumInputs(int val)
{
    ui->ninputs->setText(QString::number(val));
}

void NumInputOutputForm::setNumOutputs(int val)
{
    ui->noutputs->setText(QString::number(val));
}

void NumInputOutputForm::setNumInputsReadOnly(bool b)
{
    ui->ninputs->setReadOnly(b);
}

void NumInputOutputForm::setNumOutputsReadOnly(bool b)
{
    ui->noutputs->setReadOnly(b);
}

void QTigon::NumInputOutputForm::on_ninputs_textChanged(const QString &arg1)
{
    ///\todo Check if the input is a number or not
    Q_UNUSED(arg1);
}

void QTigon::NumInputOutputForm::on_noutputs_textChanged(const QString &arg1)
{
    ///\todo Check if the input is a number or not
    Q_UNUSED(arg1);
}
