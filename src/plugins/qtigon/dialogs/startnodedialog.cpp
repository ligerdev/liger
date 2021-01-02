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
#include <qtigon/dialogs/startnodedialog.h>
#include "ui_startnodedialog.h"

#include <QIntValidator>

StartNodeDialog::StartNodeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartNodeDialog)
{
    ui->setupUi(this);
    QIntValidator* validator = new QIntValidator;
    validator->setBottom(0);
    ui->randomSeedLineEdit->setValidator(validator);
    ui->randomSeedLineEdit->setEnabled(false);

    connect(ui->useRandomSeedCheckBox, &QCheckBox::toggled,
            ui->randomSeedLineEdit, &QLineEdit::setEnabled);
}

StartNodeDialog::~StartNodeDialog()
{
    delete ui;
}

void StartNodeDialog::setSeed(uint seed)
{
    ui->randomSeedLineEdit->setText(QString::number(seed));
}

void StartNodeDialog::setUseUserDefinedSeedCheckbox(bool b)
{
    ui->useRandomSeedCheckBox->setChecked(b);
}

bool StartNodeDialog::useCustomSeed() const
{
    return ui->useRandomSeedCheckBox->isChecked();
}

uint StartNodeDialog::seed()
{
    return static_cast<uint>(ui->randomSeedLineEdit->text().toInt());
}
