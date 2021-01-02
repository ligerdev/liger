#ifndef NUMINPUTOUTPUTFORM_H
#define NUMINPUTOUTPUTFORM_H

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
#include <QDialog>

namespace Ui {
class NumInputOutputForm;
}
namespace QTigon {
class NumInputOutputForm : public QDialog
{
    Q_OBJECT

public:
    explicit NumInputOutputForm(QWidget *parent = 0);
    ~NumInputOutputForm();
    void resetForm();
    void setNumInputs(int val);
    void setNumOutputs(int val);
    void setNumInputsReadOnly(bool b = true);
    void setNumOutputsReadOnly(bool b = true);

signals:
    void updateNumInputOutput(const int &ninput, const int &noutput);

private slots:
    void on_buttonBox_accepted();
    void on_ninputs_textChanged(const QString &arg1);
    void on_noutputs_textChanged(const QString &arg1);

private:
    Ui::NumInputOutputForm *ui;
};
}
#endif // NUMINPUTOUTPUTFORM_H
