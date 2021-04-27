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
#ifndef OUTPUTPROPERTIESDIALOG_H
#define OUTPUTPROPERTIESDIALOG_H

#include <qtigon/operators/problemgenerator/inputoutputprivatedata.h>
#include <QDialog>
#include <QMap>
#include <QPoint>
#include <QStandardItemModel>

namespace Tigon {
namespace Representation {
class ElementProperties;
}
}

namespace Ui {
class OutputPropertiesDialog;
}

namespace QTigon {

class OutputPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OutputPropertiesDialog(QWidget *parent = 0);
    /*!
     * \brief Pass a copy of private data to properties dialog
     * \param OutputPrivateData
     */
    void setData(const OutputPrivateData& data);
    /*!
     * \brief Provide a distribution map that maps a string to tigon distribution type.
     * By default, the constructor includes UniformDistType, NormalDistType,
     * LinearDistType, and PeakDistType only.
     * \param map
     */
    void setParametricDistMap(const QMap<QString, Tigon::DistributionType> &parametricDistMap);

    void setReadOnly(bool readOnly = true);

    ~OutputPropertiesDialog();

signals:
    void updateData(const OutputPrivateData& data);

private slots:
    /// Confirm button
    void on_buttonBox_accepted();

    /// Check box
    void on_objectiveCheckBox_toggled(bool checked);
    void on_constraintCheckBox_toggled(bool checked);

    /// ComboBox
    void on_distributionComboBox_currentIndexChanged(const QString &arg1);
    void on_optTypeComboBox_currentIndexChanged(const QString &arg1);

private:
    void initialise();
    void setupSignals();
    void setObj(const bool& check);
    void setCnstr(const bool& check);

    Ui::OutputPropertiesDialog *ui;
    QMap<QString, Tigon::DistributionType> m_parametricDistMap;
    QStandardItemModel         m_uncertainMappingModelData;
    OutputPrivateData          m_d;
};
}
#endif // OUTPUTPROPERTIESDIALOG_H
