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
#ifndef ENDNODEDIALOG_H
#define ENDNODEDIALOG_H

#include <QDialog>

namespace Ui {
class EndNodeDialog;
}

class EndNodeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EndNodeDialog(QWidget *parent = 0);
    ~EndNodeDialog();

    void setInputDisabled(bool b = true);
        
    qint64 nIter() const;
    qint64 budget() const;
    void   setNiter(qint64 nvar);
    void   setBudget(qint64 budget);
    bool   isUseBuget() const;
    bool   isUseIteration() const;
    bool   isEstimateTime() const;
    void   setUseBugdget(bool b=true);
    void   setUseIter(bool b=true);
    void   setEstimateTime(bool b=false);
    bool   isPauseByIteration() const;
    bool   isPauseByBudget() const;
    void   setPauseByIteration(bool b=true);
    void   setPauseByBudget(bool b=true);
    void   setLoggingPopulation(bool b=false);
    bool   isLoggingPopulation() const;
    void   setKeepPopulationsHistory(bool b=false);
    bool   isKeepPopulationsHistory() const;
    void   setLoggingEvaluation(bool b=false);
    bool   isLoggingEvaluation() const;
    void   setPopulationsLogPath(const QString& path);
    QString populationsLogPath() const;
    void   setEvaluationsLogPath(const QString& path);
    QString evaluationsLogPath() const;

    QString iterPausePoints() const;
    QString budgetPausePoints() const;
    void setIterPausePoints(const QString& points);
    void setBudgetPausePoints(const QString& points);
    QString iterPauseType() const;
    QString budgetPauseType() const;
    void setIterPauseType(const QString& type);
    void setBudgetPauseType(const QString& type);

private slots:
    void on_iterPauseLineEdit_editingFinished();
    void on_budgetPauseLineEdit_editingFinished();
    void on_iterLineEdit_editingFinished();
    void on_budgetLineEdit_editingFinished();
    void on_logPopulationCheckBox_toggled(bool checked);
    void on_logEvaluationCheckBox_toggled(bool checked);
    void on_savePopulationsLogAsPushButton_clicked();
    void on_saveEvaluationsLogAsPushButton_clicked();

private:
    Ui::EndNodeDialog *ui;
    void processStringList(QStringList& strlist);
};

#endif // ENDNODEDIALOG_H
