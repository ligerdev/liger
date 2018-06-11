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
#ifndef SELECTPLOTVARSFORM_H
#define SELECTPLOTVARSFORM_H
#include <QDialog>
#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractButton>

namespace Ui {
class SelectPlotVarsForm;
}

class QStandardItemModel;

class SelectPlotVarsForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit SelectPlotVarsForm(QWidget *parent = 0);
    ~SelectPlotVarsForm();

    void setAllNames(const QStringList& names,
                     const QVariantList& categories = QVariantList(),
                     const QVariantList& selectedIndices =  QVariantList());
    /*!
     * \brief Return the selected variables
     */
    QStringList    selectedVars() const;
    QStringList    robustnessIndicators() const;
    QVector<qreal> robustnessIndicatorsParamters() const;
    
signals:
    void VariablesUpdated();
    void RobustnessIndicatorsUpdated(const QStringList& indicators,
                                     const QVector<qreal>& parameters);

private slots:
    void on_m_addVarsButton_clicked();
    void on_m_removeVarsButton_clicked();
    void selectFlipA();
    void selectFlipB();
    void parametersChanged();
    void onPlotVarsSelected();
    void on_m_saveRobustnessButton_clicked();
    void on_m_robustnessCombo_currentIndexChanged(const QString& name);
    void on_m_buttonBox_clicked(QAbstractButton *button);

private:
    void updateParameterTable(const QString& name, const qreal& value=0.5);
    void updatePlotVarsListWidget();
    Ui::SelectPlotVarsForm *ui;
    QStringList m_allNames;
    QStringList m_selNames;
    QStringListModel* m_robustnessModel;
    QMap<QString, QString> m_robustnessMap;
    QMap<QString, qreal>   m_robustnessParamMap;
    QStandardItemModel*    m_robustParamModel;
    bool                   m_unSavedChanged;
    QVector<QColor>        m_colormap;
    QVector<QColor>        m_textColors;
    int                    m_previousRobustnessChoice;
};

#endif // SELECTPLOTVARSFORM_H
