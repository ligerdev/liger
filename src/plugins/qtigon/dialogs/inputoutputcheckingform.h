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
#ifndef INPUTOUTPUTCHECKINGFORM_H
#define INPUTOUTPUTCHECKINGFORM_H

#include <QDialog>
#include <QStandardItemModel>
#include <tigon/tigon_global.h>

namespace Ui {
class InputOutputCheckingForm;
}

class ComboboxDelegate;

namespace Tigon {
namespace Representation {
class ElementProperties;
}
}

namespace QTigon{
class InputOutputCheckingForm : public QDialog
{
    Q_OBJECT

public:
    explicit InputOutputCheckingForm(QWidget *parent = 0);
    ~InputOutputCheckingForm();

    void setFunction(Tigon::Representation::IFunctionSPtr function,
                     const QVector<int>& inputDuplications,
                     const QVector<int>& outputDuplications);

    Tigon::Representation::IFunctionSPtr function() const;

    QVector<int> inputDuplications() const;
    QVector<int> outputDuplications() const;

    int numInputMerge()  const;
    int numOutputMerge() const;

signals:
    void duplicationHandled();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_confirmButtonBox_accepted();
    void on_confirmButtonBox_rejected();
    void on_mergeAllPushButton_clicked();
    void on_renameAllPushButton_clicked();

private:
    Tigon::Representation::ElementProperties createElemPrpt(const QString& name,
                                                            Tigon::Representation::ElementProperties oldElem);
    void highlight();
    Ui::InputOutputCheckingForm* ui;
    Tigon::Representation::IFunctionSPtr  m_func;
    QStandardItemModel                 m_dataModel;
    QVector<int> m_inputDuplications;
    QVector<int> m_outputDuplications;
    int          m_numInputMerge;
    int          m_numOutputMerge;
    ComboboxDelegate*  m_delegate;
};

}

#endif // INPUTOUTPUTCHECKINGFORM_H
