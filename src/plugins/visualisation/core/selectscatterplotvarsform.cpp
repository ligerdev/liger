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
#include "selectscatterplotvarsform.h"
#include "ui_selectscatterplotvarsform.h"

SelectScatterPlotVarsForm::SelectScatterPlotVarsForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SelectScatterPlotVarsForm)
{
    ui->setupUi(this);
    m_selectedIndices.clear();
    m_selectedIndices.push_back(1);
}

SelectScatterPlotVarsForm::~SelectScatterPlotVarsForm()
{
    delete ui;
}

void SelectScatterPlotVarsForm::setXNames(const QStringList &names)
{
    ui->m_xComboBox->clear();
    ui->m_xComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 0) {
        int s = m_selectedIndices[0];
        if(ui->m_xComboBox->count() > s) {
            ui->m_xComboBox->setCurrentIndex(s);
        }
    }
}

void SelectScatterPlotVarsForm::setYNames(const QStringList &names)
{
    ui->m_yComboBox->clear();
    ui->m_yComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 1) {
        int s = m_selectedIndices[1];
        if(ui->m_yComboBox->count() > s) {
            ui->m_yComboBox->setCurrentIndex(s);
        }
    }
}

void SelectScatterPlotVarsForm::setSizeNames(const QStringList &names)
{
    ui->m_sizeComboBox->clear();
    ui->m_sizeComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 2) {
        int s = m_selectedIndices[2];
        if(ui->m_sizeComboBox->count() > s) {
            ui->m_sizeComboBox->setCurrentIndex(s);
        }
    }
}

void SelectScatterPlotVarsForm::setColorNames(const QStringList &names)
{
    ui->m_colorComboBox->clear();
    ui->m_colorComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 3) {
        int s = m_selectedIndices[3];
        if(ui->m_colorComboBox->count() > s) {
            ui->m_colorComboBox->setCurrentIndex(s);
        }
    }
}

void SelectScatterPlotVarsForm::setShowSize(bool flag)
{
    ui->m_showSize->setChecked(flag);
}

void SelectScatterPlotVarsForm::setShowColor(bool flag)
{
    ui->m_showColor->setChecked(flag);
}

bool SelectScatterPlotVarsForm::showSize() const
{
    return ui->m_showSize->isChecked();
}

bool SelectScatterPlotVarsForm::showColor() const
{
    return ui->m_showColor->isChecked();
}

QVariantList SelectScatterPlotVarsForm::selectedIndex() const
{
    QVariantList selected;
    selected.append(ui->m_xComboBox->currentIndex());
    selected.append(ui->m_yComboBox->currentIndex());
    selected.append(ui->m_sizeComboBox->currentIndex());
    selected.append(ui->m_colorComboBox->currentIndex());
    return selected;
}


void SelectScatterPlotVarsForm::setSelectedIndices(const QVariantList &selected)
{
    if(selected.size() < 4) {
        return;
    }

    m_selectedIndices.clear();
    m_selectedIndices.reserve(selected.size());
    for(auto elem : selected) {
        m_selectedIndices.push_back(elem.toInt());
    }

    if(ui->m_xComboBox->count() > m_selectedIndices[0]) {
        ui->m_xComboBox->setCurrentIndex(m_selectedIndices[0]);
    }

    if(ui->m_yComboBox->count() > m_selectedIndices[1]) {
        ui->m_yComboBox->setCurrentIndex(m_selectedIndices[1]);
    }

    if(ui->m_sizeComboBox->count() > m_selectedIndices[2]) {
        ui->m_sizeComboBox->setCurrentIndex(m_selectedIndices[2]);
    }

    if(ui->m_colorComboBox->count() > m_selectedIndices[3]) {
        ui->m_colorComboBox->setCurrentIndex(m_selectedIndices[3]);
    }
}
