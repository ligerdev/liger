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
#include "selectplotvarsform.h"
#include "ui_selectplotvarsform.h"
#include <visualisation/visualisationconstants.h>

#include <QStandardItemModel>
#include <QMessageBox>
#include <QBrush>
#include <QDebug>

using namespace Visualisation;

SelectPlotVarsForm::SelectPlotVarsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectPlotVarsForm),
    m_allNames(),
    m_selNames(),
    m_robustnessModel(new QStringListModel(this)),
    m_robustParamModel(new QStandardItemModel()),
    m_unSavedChanged(false),
    m_colormap(7),
    m_previousRobustnessChoice(0)
{
    ui->setupUi(this);

    ui->m_availableVarsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->m_plotVarsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->m_availableVarsListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_plotVarsListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_robustnessCombo->setEnabled(false);
    ui->m_robustnessCombo->setEditable(false);
    ui->m_saveRobustnessButton->setEnabled(false);
    ui->m_robustnessParamsTable->setModel(m_robustParamModel);
    ui->m_robustnessParamsTable->verticalHeader()->hide();
    connect(m_robustParamModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(parametersChanged()));

    // Set robustness
    QStringList robustnessItems;
    robustnessItems.append(QString(""));
    robustnessItems.append(QString(Constants::INDICATOR_CONFIDENCE));
    robustnessItems.append(QString(Constants::INDICATOR_THRESHOLD));
    ui->m_robustnessCombo->addItems(robustnessItems);

    QStringList robustnessTableLabels;
    robustnessTableLabels.append(QString("Parameter"));
    robustnessTableLabels.append(QString("Value"));
    m_robustParamModel->setHorizontalHeaderLabels(robustnessTableLabels);
    ui->m_robustnessParamsTable->horizontalHeader()->stretchLastSection();

    // Set the colormap
    m_colormap[0].setRgb(119, 172, 48);
    m_colormap[1].setRgb(237, 177, 32);
    m_colormap[2].setRgb(162, 20, 47);
    m_colormap[3].setRgb(0, 114, 189);
    m_colormap[4].setRgb(217, 83, 25);
    m_colormap[5].setRgb(126, 47, 142);
    m_colormap[6].setRgb(77, 190, 238);

    connect(ui->m_plotVarsListWidget->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onPlotVarsSelected()));
    connect(ui->m_availableVarsListWidget,SIGNAL(clicked(QModelIndex)),
            this,SLOT(selectFlipA()));
    connect(ui->m_plotVarsListWidget,SIGNAL(clicked(QModelIndex)),
            this,SLOT(selectFlipB()));
}

SelectPlotVarsForm::~SelectPlotVarsForm()
{
    delete ui;
}

void SelectPlotVarsForm::setAllNames(const QStringList &names,
                                     const QVariantList &categories,
                                     const QVariantList &selectedIndices)
{
    m_allNames = names;
    ui->m_availableVarsListWidget->clear();
    ui->m_availableVarsListWidget->insertItems(0, m_allNames);
    int nrows = names.size();
    m_textColors.fill(Qt::black, nrows);
    if(categories.size() == nrows) {
        for(int i=0; i<nrows; i++) {
            m_textColors[i] = m_colormap[categories[i].toInt()%7];
            ui->m_availableVarsListWidget->item(i)->setForeground(QBrush(m_textColors[i]));
        }
    }
    if(selectedIndices.size() > 0 && selectedIndices.size() <= names.size()) {
        std::transform(selectedIndices.cbegin(),
                       selectedIndices.cend(),
                       std::back_inserter(m_selNames),
                       [names](const QVariant var){return names.at(var.toInt());});
        m_selNames.removeDuplicates();
        qDebug() << m_selNames;
        updatePlotVarsListWidget();
    }
}

QStringList SelectPlotVarsForm::selectedVars() const
{
    if(m_selNames.size() == 0) {
        return QStringList("");
    } else {
        return m_selNames;
    }
}

QStringList SelectPlotVarsForm::robustnessIndicators() const
{
    QStringList out;
    foreach(const QString &name, m_selNames) {
        if(m_robustnessMap.contains(name)) {
            out.append(m_robustnessMap[name]);
        } else {
            out.append("");
        }
    }
    return out;
}

QVector<qreal> SelectPlotVarsForm::robustnessIndicatorsParamters() const
{
    QVector<qreal> out;
    foreach(QString name, m_selNames) {
        if(m_robustnessParamMap.contains(name)) {
            out.append(m_robustnessParamMap[name]);
        } else {
            out.append(0.0);
        }
    }
    return out;
}

void SelectPlotVarsForm::on_m_addVarsButton_clicked()
{
    foreach(const QModelIndex &index,
            ui->m_availableVarsListWidget->selectionModel()->selectedIndexes()) {
        m_selNames.append(m_allNames.at(index.row()));
    }
    m_selNames.removeDuplicates();
    updatePlotVarsListWidget();
    emit VariablesUpdated();
}

void SelectPlotVarsForm::on_m_removeVarsButton_clicked()
{
    qDeleteAll(ui->m_plotVarsListWidget->selectedItems());

    m_selNames.clear();
    for(int i=0; i<ui->m_plotVarsListWidget->count(); i++) {
        m_selNames.append(ui->m_plotVarsListWidget->model()->index(i,0)
                          .data().toString());
    }

    int count = 0;
    foreach (const QString& name, m_selNames) {
        int index = m_allNames.indexOf(name);
        QBrush brush(m_textColors[index]);
        ui->m_plotVarsListWidget->item(count)->setForeground(brush);
        ++count;
    }

    emit VariablesUpdated();
}

void SelectPlotVarsForm::selectFlipA()
{
    ui->m_plotVarsListWidget->clearSelection();
}

void SelectPlotVarsForm::selectFlipB()
{
    ui->m_availableVarsListWidget->clearSelection();
}

void SelectPlotVarsForm::parametersChanged()
{
    m_unSavedChanged = true;
}

void SelectPlotVarsForm::onPlotVarsSelected()
{
    QModelIndexList selectedList =
            ui->m_plotVarsListWidget->selectionModel()->selectedIndexes();

    // No item selected
    if(selectedList.size() == 0) {
        ui->m_robustnessCombo->setCurrentIndex(-1);
        ui->m_robustnessCombo->setEnabled(false);
        ui->m_saveRobustnessButton->setEnabled(false);
        return;
    }

    // Items selected
    ui->m_robustnessCombo->setEnabled(true);
    ui->m_saveRobustnessButton->setEnabled(true);

    // Work out if all the variables are of the same robustness type
    QStringList list;
    QVector<qreal> value;
    foreach(QModelIndex item, selectedList) {
        QString key = item.data().toString();
        list.append(m_robustnessMap[key]);
        value.append(m_robustnessParamMap[key]);
    }

    bool isNameUnique = true;
    if(list.size() > 1) {
        foreach(QString str1, list) {
            foreach(QString str2, list) {
                if(str1!=str2) {
                    isNameUnique = false;
                    break;
                }
            }
        }
    }

    if(isNameUnique) {
        ui->m_robustnessCombo->setCurrentText(list[0]);
        bool isValueUnique = true;
        if(value.size() > 1) {
            foreach(qreal val1, value) {
                foreach(qreal val2, value) {
                    if(val1!=val2) {
                        isValueUnique = false;
                        break;
                    }
                }
            }
        }
        if(isValueUnique) {
            updateParameterTable(list[0], value[0]);
        } else {
            updateParameterTable("");
        }
    } else {
        ui->m_robustnessCombo->setCurrentIndex(-1);
    }

    return;
}

void SelectPlotVarsForm::on_m_saveRobustnessButton_clicked()
{
    m_unSavedChanged = false;
    QModelIndexList selectedList =
            ui->m_plotVarsListWidget->selectionModel()->selectedIndexes();

    if(selectedList.size()>0) {
        foreach(QModelIndex item, selectedList) {
            m_robustnessMap[item.data().toString()] = ui->m_robustnessCombo->currentText();
            m_robustnessParamMap[item.data().toString()] =
                    m_robustParamModel->index(0,1).data().toReal();
        }
        emit RobustnessIndicatorsUpdated(robustnessIndicators(),
                                         robustnessIndicatorsParamters());
    }
}

void SelectPlotVarsForm::updateParameterTable(const QString &name,
                                              const qreal &value)
{
    if(name == QString(Constants::INDICATOR_CONFIDENCE)) {
        m_robustParamModel->setItem(0,0,new QStandardItem("Conf. Level"));
        m_robustParamModel->setItem(0,1,
                                    new QStandardItem(
                                        QString::number(value)));
        m_robustParamModel->item(0,0)->setEditable(false);

    } else if(name == QString(Constants::INDICATOR_THRESHOLD)) {
        m_robustParamModel->setItem(0,0,new QStandardItem("Threshold"));
        m_robustParamModel->setItem(0,1,
                                    new QStandardItem(
                                        QString::number(value)));
        m_robustParamModel->item(0,0)->setEditable(false);

    } else {
        m_robustParamModel->setItem(0,0,new QStandardItem(""));
        m_robustParamModel->setItem(0,1,new QStandardItem(""));
        m_robustParamModel->item(0,0)->setEditable(false);
    }
}

void SelectPlotVarsForm::updatePlotVarsListWidget()
{
    ui->m_plotVarsListWidget->clear();
    qDebug() << m_selNames;
    ui->m_plotVarsListWidget->insertItems(0, m_selNames);
    int count = 0;
    foreach (const QString& name, m_selNames) {
        int index = m_allNames.indexOf(name);
        QBrush brush(m_textColors[index]);
        ui->m_plotVarsListWidget->item(count)->setForeground(brush);
        ++count;
    }
}

void SelectPlotVarsForm::on_m_robustnessCombo_currentIndexChanged(const QString &name)
{
    if(isVisible()) {
        int currentIdx = ui->m_robustnessCombo->currentIndex();
        if(m_previousRobustnessChoice != currentIdx) {
            m_unSavedChanged = true;
            m_previousRobustnessChoice = currentIdx;
            updateParameterTable(name);
        }
    }
}

void SelectPlotVarsForm::on_m_buttonBox_clicked(QAbstractButton *button)
{
    // Save the changes if unsaved
    if(button == ui->m_buttonBox->button(QDialogButtonBox::Ok)) {
        if(m_unSavedChanged) {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                      "Robustness Indicators",
                                                                      "Do you want to save the changes?",
                                                                      QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                ui->m_saveRobustnessButton->click();
            }
        }
    }
}
