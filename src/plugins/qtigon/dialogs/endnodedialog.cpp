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
#include <qtigon/dialogs/endnodedialog.h>
#include "ui_endnodedialog.h"

#include <algorithm>

#include <QStringList>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QIntValidator>

EndNodeDialog::EndNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EndNodeDialog)
{
    ui->setupUi(this);
    setNiter(0);
    setBudget(0);
    connect(ui->iterCheckBox, SIGNAL(toggled(bool)),
            ui->iterLineEdit, SLOT(setEnabled(bool)));
    connect(ui->budgetCheckbox, SIGNAL(toggled(bool)),
            ui->budgetLineEdit, SLOT(setEnabled(bool)));
    ui->iterLineEdit->setDisabled(true);
    ui->budgetLineEdit->setDisabled(true);

    connect(ui->iterPauseCheckBox, SIGNAL(toggled(bool)),
            ui->iterPauseLineEdit, SLOT(setEnabled(bool)));
    connect(ui->budgetPauseCheckbox, SIGNAL(toggled(bool)),
            ui->budgetPauseLineEdit, SLOT(setEnabled(bool)));
    connect(ui->iterPauseCheckBox, SIGNAL(toggled(bool)),
            ui->iterPauseComboBox, SLOT(setEnabled(bool)));
    connect(ui->budgetPauseCheckbox, SIGNAL(toggled(bool)),
            ui->budgetPauseComboBox, SLOT(setEnabled(bool)));
    ui->iterPauseLineEdit->setDisabled(true);
    ui->budgetPauseLineEdit->setDisabled(true);
    ui->iterPauseComboBox->setDisabled(true);
    ui->budgetPauseComboBox->setDisabled(true);
    ui->populationsLogPathlineEdit->setDisabled(true);
    ui->savePopulationsLogAsPushButton->setDisabled(true);
    ui->evaluationsLogPathlineEdit->setDisabled(true);
    ui->saveEvaluationsLogAsPushButton->setDisabled(true);
    QValidator *validator = new QIntValidator(0, 999999999, this);
    ui->iterLineEdit->setValidator(validator);
    ui->budgetLineEdit->setValidator(validator);
    ui->populationsLogPathlineEdit->setReadOnly(true);
    ui->evaluationsLogPathlineEdit->setReadOnly(true);
    ui->tabWidget->setCurrentIndex(0);
}

EndNodeDialog::~EndNodeDialog()
{
    delete ui;
}

void EndNodeDialog::setInputDisabled(bool b)
{
    ui->terminationTab->setDisabled(b);
    ui->pauseTab->setDisabled(b);
    ui->loggingTab->setDisabled(b);
    ui->buttonBox->setDisabled(b);
}

qint64 EndNodeDialog::nIter() const
{
    return ui->iterLineEdit->text().toInt();
}

qint64 EndNodeDialog::budget() const
{
    return ui->budgetLineEdit->text().toInt();
}

void EndNodeDialog::setNiter(qint64 nvar)
{
    if(nvar > 0) {
        ui->iterLineEdit->setText(QString::number(nvar));
    } else {
        ui->iterLineEdit->setText(QString::number(0));
    }
}

void EndNodeDialog::setBudget(qint64 budget)
{
    if(budget > 0) {
        ui->budgetLineEdit->setText(QString::number(budget));
    } else {
        ui->budgetLineEdit->setText(QString::number(0));
    }
}

bool EndNodeDialog::isUseBuget() const
{
    return ui->budgetCheckbox->isChecked();
}

bool EndNodeDialog::isUseIteration() const
{
    return ui->iterCheckBox->isChecked();
}

bool EndNodeDialog::isEstimateTime() const
{
    return ui->estimateTime->isChecked();
}

bool EndNodeDialog::isPauseByIteration() const
{
    return ui->iterPauseCheckBox->isChecked();
}

bool EndNodeDialog::isPauseByBudget() const
{
    return ui->budgetPauseCheckbox->isChecked();
}

void EndNodeDialog::setPauseByIteration(bool b)
{
    ui->iterPauseCheckBox->setChecked(b);
}

void EndNodeDialog::setPauseByBudget(bool b)
{
    ui->budgetPauseCheckbox->setChecked(b);
}

void EndNodeDialog::setLoggingPopulation(bool b)
{
    ui->logPopulationCheckBox->setChecked(b);
}

bool EndNodeDialog::isLoggingPopulation() const
{
    return ui->logPopulationCheckBox->isChecked();
}

void EndNodeDialog::setKeepPopulationsHistory(bool b)
{
    ui->keepPopulationHistoryCheckBox->setChecked(b);
}

bool EndNodeDialog::isKeepPopulationsHistory() const
{
    return ui->keepPopulationHistoryCheckBox->isChecked();
}

void EndNodeDialog::setLoggingEvaluation(bool b)
{
    ui->logEvaluationCheckBox->setChecked(b);
}

bool EndNodeDialog::isLoggingEvaluation() const
{
    return ui->logEvaluationCheckBox->isChecked();
}

void EndNodeDialog::setPopulationsLogPath(const QString &path)
{
    ui->populationsLogPathlineEdit->setText(path);
}

QString EndNodeDialog::populationsLogPath() const
{
    return ui->populationsLogPathlineEdit->text();
}

void EndNodeDialog::setEvaluationsLogPath(const QString &path)
{
    ui->evaluationsLogPathlineEdit->setText(path);
}

QString EndNodeDialog::evaluationsLogPath() const
{
    return ui->evaluationsLogPathlineEdit->text();
}

void EndNodeDialog::setUseBugdget(bool b)
{
    ui->budgetCheckbox->setChecked(b);
}

void EndNodeDialog::setUseIter(bool b)
{
    ui->iterCheckBox->setChecked(b);
}

void EndNodeDialog::setEstimateTime(bool b)
{
    ui->estimateTime->setChecked(b);
}

QString EndNodeDialog::iterPausePoints() const
{
    return ui->iterPauseLineEdit->text();
}

QString EndNodeDialog::budgetPausePoints() const
{
    return ui->budgetPauseLineEdit->text();
}

void EndNodeDialog::setIterPausePoints(const QString &points)
{
    ui->iterPauseLineEdit->setText(points);
}

void EndNodeDialog::setBudgetPausePoints(const QString &points)
{
    ui->budgetPauseLineEdit->setText(points);
}

QString EndNodeDialog::iterPauseType() const
{
    return ui->iterPauseComboBox->currentText();
}

QString EndNodeDialog::budgetPauseType() const
{
    return ui->budgetPauseComboBox->currentText();
}

void EndNodeDialog::setIterPauseType(const QString &type)
{
    ui->iterPauseComboBox->setCurrentText(type);
}

void EndNodeDialog::setBudgetPauseType(const QString &type)
{
    ui->budgetPauseComboBox->setCurrentText(type);
}

void EndNodeDialog::processStringList(QStringList &strlist)
{
    std::list<int> tmpInt;
    std::transform(strlist.begin(), strlist.end(),
                   std::back_inserter(tmpInt),
                   [](const QString& s){return s.toInt();});
    tmpInt.sort();
    std::list<int>::iterator it
            = std::remove_if(tmpInt.begin(), tmpInt.end()
                             , [](int t){return (t <= 0);});
    tmpInt.erase(it, tmpInt.end());
    strlist.clear();
    std::transform(tmpInt.begin(), tmpInt.end(), std::back_inserter(strlist),
                   [](const int& t){return QString::number(t);});
    strlist.removeDuplicates();
}

void EndNodeDialog::on_iterPauseLineEdit_editingFinished()
{
    QString content = ui->iterPauseLineEdit->text();
    content = content.replace(" ", "");
    QStringList contentList = content.split(";");
    processStringList(contentList);
    ui->iterPauseLineEdit->setText(contentList.join(";"));
}

void EndNodeDialog::on_budgetPauseLineEdit_editingFinished()
{
    QString content = ui->budgetPauseLineEdit->text();
    content = content.replace(" ", "");
    QStringList contentList = content.split(";");
    contentList.removeDuplicates();
    for(int i=0; i<contentList.size(); i++) {
        if(contentList[i].isEmpty()) {
            contentList.removeAt(i);
        }
    }
    processStringList(contentList);
    ui->budgetPauseLineEdit->setText(contentList.join(";"));
}

void EndNodeDialog::on_iterLineEdit_editingFinished()
{
    int input = ui->iterLineEdit->text().toInt();
    if(input < 1) {
        ui->iterLineEdit->setText("0");
    }
}

void EndNodeDialog::on_budgetLineEdit_editingFinished()
{
    int input = ui->budgetLineEdit->text().toInt();
    if(input < 1) {
        ui->budgetLineEdit->setText("0");
    }
}

void EndNodeDialog::on_logPopulationCheckBox_toggled(bool checked)
{
    if(checked) {
        ui->populationsLogPathlineEdit->setEnabled(true);
        ui->keepPopulationHistoryCheckBox->setEnabled(true);
        ui->savePopulationsLogAsPushButton->setEnabled(true);
    } else {
        ui->populationsLogPathlineEdit->setEnabled(false);
        ui->keepPopulationHistoryCheckBox->setEnabled(false);
        ui->keepPopulationHistoryCheckBox->setChecked(false);
//        ui->populationsLogPathlineEdit->clear();
        ui->savePopulationsLogAsPushButton->setEnabled(false);
    }
}

void EndNodeDialog::on_logEvaluationCheckBox_toggled(bool checked)
{
    if(checked) {
        ui->evaluationsLogPathlineEdit->setEnabled(true);
        ui->saveEvaluationsLogAsPushButton->setEnabled(true);
    } else {
        ui->evaluationsLogPathlineEdit->setEnabled(false);
//        ui->evaluationsLogPathlineEdit->clear();
        ui->saveEvaluationsLogAsPushButton->setEnabled(false);
    }
}

void EndNodeDialog::on_savePopulationsLogAsPushButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save Populations File As"),
                                                QDir::homePath(),
                                                QString("Json (*.json);;"
                                                        "Text (*.txt)"));
    if(path.isEmpty()) {
        return;
    }
    ui->populationsLogPathlineEdit->setText(path);
}

void EndNodeDialog::on_saveEvaluationsLogAsPushButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save Evaluations File As"),
                                                QDir::homePath(),
                                                QString("Json (*.json);;"
                                                        "Text (*.txt)"));
    if(path.isEmpty()) {
        return;
    }
    ui->evaluationsLogPathlineEdit->setText(path);
}
