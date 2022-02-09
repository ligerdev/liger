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
#ifndef INPUTPROPERTIESDIALOG_H
#define INPUTPROPERTIESDIALOG_H

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
class InputPropertiesDialog;
}

namespace QTigon {
/// \todo Simplify this class. Only make changes when confirm button is clicked.
class InputPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputPropertiesDialog(QWidget *parent = 0);
    /*!
     * \brief Pass a copy of private data to properties dialog
     * \param InputPrivateData
     */
    void setData(const InputPrivateData& data);
    /*!
     * \brief Provide a distribution map that maps a string to tigon distribution type.
     * By default, the constructor includes UniformDistType, NormalDistType,
     * LinearDistType, and PeakDistType only.
     * \param map
     */
    void setDistributionMaps(const QMap<QString, Tigon::DistributionType>& map);
    void setParametricDistMap(const QMap<QString, Tigon::DistributionType> &parametricDistMap);

    void setReadOnly(bool readOnly = true);

    ~InputPropertiesDialog();

signals:
    void updateData(const InputPrivateData& data);

private slots:
    void updateUncertaintyMappingParameters(const QModelIndex& topleft,
                                            const QModelIndex& bottomright,
                                            const QVector<int>& roles);
    void updateParameterDistData(const QModelIndex& topleft,
                                 const QModelIndex& bottomright,
                                 const QVector<int>& roles);
    void customContextMenuForNonParametricDist(const QPoint& point);
    void appendOneRow();
    void insertAbove();
    void insertBelow();
    void removeOneRow();
    void loadRawData();

    /// Button box
    void on_buttonBox_accepted();

    /// Line editor
    void on_parameterValueLineEdit_textChanged(const QString &arg1);
    void on_lboundLineEdit_textChanged(const QString &arg1);
    void on_uBoundlineEdit_textChanged(const QString &arg1);

    /// Comboboxes
    void on_typeComboBox_currentIndexChanged(int index);
    void on_distComboBox_currentIndexChanged(const QString &arg1);

    /// Radio buttons
    void on_uncertaintyRadioButton_toggled(bool checked);
    void on_deterministicRadioButton_toggled(bool checked);
    void on_variableRadioButton_toggled(bool checked);
    void on_paramRadioButton_toggled(bool checked);
    void on_isExternalCheckBox_toggled(bool checked);

private:
    void initialise();
    void setupSignals();
    void configureDistComboBox(QMap<QString, Tigon::DistributionType> distMap);
    void buildParamterDistributionView();
    void buildVariableDistributionView();
    void showDetermisticVariablePrpt();
    void showUncertainVariablePrpt();
    void showDetermisticParameterPrpt();
    void showUncertainParameterPrpt();

    /// Private data
    Ui::InputPropertiesDialog *ui;
    QMap<QString, Tigon::DistributionType> m_allDistMap;
    QMap<QString, Tigon::DistributionType> m_parametricDistMap;
    QStandardItemModel        m_uncertainMappingModelData;
    QStandardItemModel        m_parameterDistModelData;
    InputPrivateData          m_d;
};

}

#endif // INPUTPROPERTIESDIALOG_H
