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
#ifndef QOPERATORDIAG_H
#define QOPERATORDIAG_H

#include <QWidget>
#include <QAction>
#include <QStandardItemModel>

namespace Ui {
class QOperatorDiag;
}

namespace Tigon {
namespace Operators {
class IOperator;
}
}

class QOperatorDiagTabItem;

class QOperatorDiag : public QWidget
{
    Q_OBJECT

public:
    explicit QOperatorDiag(QWidget *parent = 0);
    ~QOperatorDiag();
    void setup(Tigon::Operators::IOperator* thisOp);
    void setInputDisabled(bool b = true);
    /*!
     * \brief Add a tool widget in the dialog.
     * \param toolWidget
     */
    void addToolWidget(QOperatorDiagTabItem* toolWidget, const QString& name);
    void clear();

signals:
    void opDataChanged();

public slots:
    void upadtePropertyDisplayData();

private slots:
    void upadtePropertyTable(const QModelIndex &index);
    void updateOperatorConfig();
    void saveConfig();
    /*!
     * \brief Discard the changes for the current property
     */
    void on_resetThisButton_clicked();
    void on_resetAllButton_clicked();
    void on_modifyDataButton_clicked();

private:
    void loadPropertiesFromOp();
    Ui::QOperatorDiag *ui;
    Tigon::Operators::IOperator* m_op;
    QStandardItemModel* m_dataModel;
};

#endif // QOPERATORDIAG_H
