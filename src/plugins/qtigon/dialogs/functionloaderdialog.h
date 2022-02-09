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
#ifndef FUNCTIONLOADERDIALOG_H
#define FUNCTIONLOADERDIALOG_H

#include <QDialog>
#include <tigon/tigon_global.h>

namespace Ui {
class FunctionLoaderDialog;
}

namespace Tigon {
namespace Representation {
class IFunction;
}
}

namespace QTigon {
class NumInputOutputForm;
class FunctionLoaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FunctionLoaderDialog(QWidget *parent = 0,
                                  const QStringList &funcNames = QStringList());
    ~FunctionLoaderDialog();
    void setFunctionNames(const QStringList &funcNames); // take a copy of probNames

signals:
    /*!
     * \brief functionSelected
     * \param The actual class name of the function is passed, e.g.
     *        Tigon::Representation::DTLZ1, not just DTLZ1. This name can be
     *        obtained by calling function->metaObject()->className();
     */
    void functionSelected(const QString& className,
                          const QString& funcType,
                          const int &ninput  = -1,
                          const int &noutput = -1);

private slots:
    void on_funcComboBox_currentIndexChanged(const QString &func);
    void on_comfirmButtonBox_accepted();
    void updateNumInputOutput(const int &ninput, const int &noutput);
    void on_editButton_clicked();
    void on_searchMFilePushButton_clicked();
    void on_searchPyFilePushButton_clicked();

private:
    void resetDetails();
    void loadFunctionInfoToView();
    void initialiseDetailsPanel();
    Ui::FunctionLoaderDialog *ui;
    NumInputOutputForm       *m_numForm;
    Tigon::Representation::IFunctionSPtr m_currentFunc;
};

}

#endif // FunctionLoaderDialog_H
