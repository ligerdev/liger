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
#include "functionloaderdialog.h"
#include "ui_functionloaderdialog.h"
#include <qtigon/dialogs/numinputoutputform.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Operators/Formulations/ProblemGenerator/ProblemGenerator.h>

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

using namespace Tigon;
using namespace Tigon::Representation;
using Tigon::Operators::ProblemGenerator;

namespace QTigon {

FunctionLoaderDialog::FunctionLoaderDialog(QWidget *parent,
                                           const QStringList &funcNames)
    : QDialog(parent)
    , ui(new Ui::FunctionLoaderDialog)
    , m_numForm(new NumInputOutputForm)
    , m_currentFunc(0)
{
    ui->setupUi(this);
    setFunctionNames(funcNames);
    connect(m_numForm, SIGNAL(updateNumInputOutput(int,int)),
            this, SLOT(updateNumInputOutput(int,int)));
    ui->editButton->setVisible(false);
    initialiseDetailsPanel();
}

FunctionLoaderDialog::~FunctionLoaderDialog()
{
    delete ui;
    delete m_numForm;
}

void FunctionLoaderDialog::setFunctionNames(const QStringList &funcNames)
{
    if(ui->funcComboBox->count() > 0) {
        ui->funcComboBox->clearEditText();
    }
    ui->funcComboBox->addItems(funcNames);
}

void FunctionLoaderDialog::on_funcComboBox_currentIndexChanged(const QString &func)
{
    if(!func.isEmpty()) {
        QString name("Tigon::Representation::"+func);
        m_currentFunc = ProblemGenerator::createFunction(FuncTypeStrInternal,
                                                         name.toStdString());
        if(m_currentFunc) {
            loadFunctionInfoToView();
        } else {
            QString message("Cannot find " + func);
            QMessageBox::warning(this, tr("Function Loader"), message,
                                 QMessageBox::Ok);
            resetDetails();
        }
    }
}

void FunctionLoaderDialog::initialiseDetailsPanel()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->mFilePathLineEdit->setReadOnly(true);
    ui->funcComboBox->setCurrentIndex(0);
    ui->funcNameLineEdit->setReadOnly(true);
    ui->funcDesTextEdit->setReadOnly(true);
}

void FunctionLoaderDialog::resetDetails()
{
    ui->funcComboBox->setCurrentIndex(0);
    ui->funcNameLineEdit->clear();
    ui->funcDesTextEdit->clear();
    ui->mFilePathLineEdit->clear();
}

void FunctionLoaderDialog::loadFunctionInfoToView()
{
    ui->funcNameLineEdit->setText(QString::fromStdString(m_currentFunc->name()));
    ui->funcDesTextEdit->setText(QString::fromStdString(m_currentFunc->description()));

    bool isInputModifiable  = m_currentFunc->isNumInputsModifiable();
    bool isOutputModifiable = m_currentFunc->isNumOutputsModifiable();

    if(isInputModifiable || isOutputModifiable) {
        ui->editButton->setVisible(true);
        m_numForm->resetForm();
        if(!isInputModifiable) {
            m_numForm->setNumInputs(m_currentFunc->TP_nInputs());
            m_numForm->setNumInputsReadOnly();
        }
        if(!isOutputModifiable) {
            m_numForm->setNumOutputs(m_currentFunc->TP_nOutputs());
            m_numForm->setNumOutputsReadOnly();
        }
        m_numForm->setModal(true);
        m_numForm->show();
    } else {
        ui->editButton->setVisible(false);
    }
}

void FunctionLoaderDialog::on_comfirmButtonBox_accepted()
{
    if(m_currentFunc) {
        emit functionSelected(QString::fromStdString(m_currentFunc->path()),
                              QString::fromStdString(m_currentFunc->typeStr()),
                              m_currentFunc->TP_nInputs(),
                              m_currentFunc->TP_nOutputs());
    }
}

void FunctionLoaderDialog::updateNumInputOutput(const int &ninput,
                                                const int &noutput)
{
    this->raise();
    m_currentFunc->TP_defineNInputs(ninput);
    m_currentFunc->TP_defineNOutputs(noutput);
}

void FunctionLoaderDialog::on_editButton_clicked()
{
    m_numForm->show();
}

void FunctionLoaderDialog::on_searchMFilePushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open MATLAB file"),
                                                    QDir::homePath(),
                                                    tr("MATLAB Files (*.m)"));
    if(filePath.isEmpty()) {
        return;
    }

    ui->mFilePathLineEdit->setText(filePath);
    m_currentFunc = ProblemGenerator::createFunctionByName("Tigon::Representation::MatlabFunction",
                                                           filePath.toStdString());

    if(m_currentFunc) {
        loadFunctionInfoToView();
    } else {
        QString message("Failed to load the Matlab function" + filePath);
        QMessageBox::warning(this,
                             tr("Function Loader"), message,
                             QMessageBox::Ok);
        resetDetails();
    }
}

void FunctionLoaderDialog::on_searchPyFilePushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open Python file"),
                                                    QDir::homePath(),
                                                    tr("Python Files (*.py)"));
    if(filePath.isEmpty()) {
        return;
    }

    ui->pyFilePathLineEdit->setText(filePath);
    m_currentFunc = ProblemGenerator::createFunctionByName("Tigon::Representation::PythonFunction",
                                                           filePath.toStdString());

    if(m_currentFunc) {
        loadFunctionInfoToView();
    } else {
        QString message("Failed to load the Python function" + filePath);
        QMessageBox::warning(this,
                             tr("Function Loader"), message,
                             QMessageBox::Ok);
        resetDetails();
    }
}

}
