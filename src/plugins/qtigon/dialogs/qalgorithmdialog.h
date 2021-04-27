/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef QALGORITHMDIALOG_H
#define QALGORITHMDIALOG_H

#include <QString>
#include <QFrame>
#include <QStandardItemModel>

namespace Tigon {
namespace Algorithms {
class IAlgorithm;
}
}

namespace Ui {
class QAlgorithmDialog;
}

/*!
 * \brief Dialog for viewing and configuring the properties of an algorithm.
 * All properties declared in any operator of an algorithm are extracted,
 * organised and displayed dynamically.
 * Users can view and modify some control parameters if allowed (set function of
 * the chosen property must be defined).
 */
class QAlgorithmDialog : public QFrame
{
    Q_OBJECT

public:
    explicit QAlgorithmDialog(QWidget *parent = 0);
    ~QAlgorithmDialog();
    void setup(Tigon::Algorithms::IAlgorithm* thisAlg);
    void clear();
    void setInputDisabled(bool b = true);

signals:
    void algDataChanged();

private slots:
    void upadtePropertyDisplayData(QModelIndex index);
    void updateOperatorConfig();
    void saveConfig();
    void on_resetThisButton_clicked();
    void on_resetAllButton_clicked();
    void on_modifyDataButton_clicked();

private:
    void     loadPropertiesFromAlg();
    QString  variantToString(QVariant variant) const;
    QVariant stringToVariant(QString str, QVariant::Type type) const;
    Ui::QAlgorithmDialog *ui;
    Tigon::Algorithms::IAlgorithm* m_alg;
    QStandardItemModel* m_dataModel;
};

#endif // QALGORITHMDIALOG_H
