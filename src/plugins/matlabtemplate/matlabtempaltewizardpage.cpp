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
#include "matlabtempaltewizardpage.h"
#include "ui_matlabtempaltewizardpage.h"

#include <QIntValidator>
#include <QMessageBox>
#include <QFile>

namespace MatlabTemplate {
namespace Internal {

MatlabTempalteWizardPage::MatlabTempalteWizardPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::MatlabTempalteWizardPage)
{
    ui->setupUi(this);

    setTitle("Information");
    ui->m_nInputsLlineEdit->setValidator(new QIntValidator);
    ui->m_nOutputsLineEdit->setValidator(new QIntValidator);
    registerField("functionName*", ui->m_funcNameLlineEdit);
    registerField("functionDescription", ui->m_descriptionLineEdit);
    registerField("nInputs*", ui->m_nInputsLlineEdit);
    registerField("nOutputs*", ui->m_nOutputsLineEdit);
}

MatlabTempalteWizardPage::~MatlabTempalteWizardPage()
{
    delete ui;
}

QString MatlabTempalteWizardPage::content() const
{
    QString content;
    QFile templateFile(":/matlabtemplate/template/t_matlabtemplate");

    if(!templateFile.open(QIODevice::ReadOnly)) {
        return content;
    }
    content = QString(templateFile.readAll());

    /// Func properties
    content.replace("{ FUNC_NAME }", field("functionName").toString());
    content.replace("{ FUNC_DESCRIPTION }", field("functionDescription").toString());

    /// Input properties
    QString inputName, inputDescription, inputType, inputUnit, inputLbounds, inputUbounds;
    int N = ui->m_nInputsLlineEdit->text().toInt();
    for(int i=0; i<N; i++) {
        inputName.append("\"x" + QString::number(i+1) + "\"");
        inputDescription.append("\"x" + QString::number(i+1) + "\"");
        inputType.append("\"real\"");
        inputUnit.append("\"\"");
        inputLbounds.append("0.0");
        inputUbounds.append("1.0");
        if(i < N-1) {
            inputName.append(", ");
            inputDescription.append(", ");
            inputType.append(", ");
            inputUnit.append(", ");
            inputLbounds.append(", ");
            inputUbounds.append(", ");
        }
    }

    content.replace("{ INPUT_NAMES }", inputName);
    content.replace("{ INPUT_DESCRIPTIONS }", inputDescription);
    content.replace("{ INPUT_TYPES }", inputType);
    content.replace("{ INPUT_UNITES }", inputUnit);
    content.replace("{ INPUT_LBOUNDS }", inputLbounds);
    content.replace("{ INPUT_UBOUNDS }", inputUbounds);

    /// Output properties
    QString outputName, outputDescription, outputType, outputUnit;
    N = ui->m_nOutputsLineEdit->text().toInt();
    for(int i=0; i<N; i++) {
        outputName.append("\"y" + QString::number(i+1) + "\"");
        outputDescription.append("\"y" + QString::number(i+1) + "\"");
        outputType.append("\"real\"");
        outputUnit.append("\"\"");
        if(i < N-1) {
            outputName.append(", ");
            outputDescription.append(", ");
            outputType.append(", ");
            outputUnit.append(", ");
        }
    }

    content.replace("{ OUTPUT_NAMES }", outputName);
    content.replace("{ OUTPUT_DESCRIPTIONS }", outputDescription);
    content.replace("{ OUTPUT_TYPES }", outputType);
    content.replace("{ OUTPUT_UNITS }", outputUnit);

    return content;
}

QString MatlabTempalteWizardPage::functionName() const
{
    return ui->m_funcNameLlineEdit->text();
}

}
}
