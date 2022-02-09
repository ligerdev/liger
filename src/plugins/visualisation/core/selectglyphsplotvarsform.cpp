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
#include "selectglyphsplotvarsform.h"
#include "ui_selectglyphsplotvarsform.h"

SelectGlyphsPlotVarsForm::SelectGlyphsPlotVarsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectGlyphsPlotVarsForm)
{
    ui->setupUi(this);
    m_selectedIndices.clear();
    m_selectedIndices.push_back(1);
}

SelectGlyphsPlotVarsForm::~SelectGlyphsPlotVarsForm()
{
    delete ui;
}

void SelectGlyphsPlotVarsForm::setXNames(const QStringList &names)
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

void SelectGlyphsPlotVarsForm::setYNames(const QStringList &names)
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

void SelectGlyphsPlotVarsForm::setTopNames(const QStringList &names)
{
    ui->m_topComboBox->clear();
    ui->m_topComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 2) {
        int s = m_selectedIndices[2];
        if(ui->m_topComboBox->count() > s) {
            ui->m_topComboBox->setCurrentIndex(s);
        }
    }
}

void SelectGlyphsPlotVarsForm::setBottomNames(const QStringList &names)
{
    ui->m_bottomComboBox->clear();
    ui->m_bottomComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 3) {
        int s = m_selectedIndices[3];
        if(ui->m_bottomComboBox->count() > s) {
            ui->m_bottomComboBox->setCurrentIndex(s);
        }
    }
}

void SelectGlyphsPlotVarsForm::setRightNames(const QStringList &names)
{
    ui->m_rightComboBox->clear();
    ui->m_rightComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 4) {
        int s = m_selectedIndices[4];
        if(ui->m_rightComboBox->count() > s) {
            ui->m_rightComboBox->setCurrentIndex(s);
        }
    }
}

void SelectGlyphsPlotVarsForm::setLeftNames(const QStringList &names)
{
    ui->m_leftComboBox->clear();
    ui->m_leftComboBox->insertItems(0, names);
    if(m_selectedIndices.size() > 5) {
        int s = m_selectedIndices[5];
        if(ui->m_leftComboBox->count() > s) {
            ui->m_leftComboBox->setCurrentIndex(s);
        }
    }
}

QVariantList SelectGlyphsPlotVarsForm::selectedIndex() const
{
    QVariantList selected;
    selected.append(ui->m_xComboBox->currentIndex());
    selected.append(ui->m_yComboBox->currentIndex());
    selected.append(ui->m_topComboBox->currentIndex());
    selected.append(ui->m_bottomComboBox->currentIndex());
    selected.append(ui->m_rightComboBox->currentIndex());
    selected.append(ui->m_leftComboBox->currentIndex());
    return selected;
}

void SelectGlyphsPlotVarsForm::setSelectedIndices(const QVariantList &selected)
{
    if(selected.size() < 6) {
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

    if(ui->m_topComboBox->count() > m_selectedIndices[2]) {
        ui->m_topComboBox->setCurrentIndex(m_selectedIndices[2]);
    }

    if(ui->m_bottomComboBox->count() > m_selectedIndices[3]) {
        ui->m_bottomComboBox->setCurrentIndex(m_selectedIndices[3]);
    }

    if(ui->m_rightComboBox->count() > m_selectedIndices[4]) {
        ui->m_rightComboBox->setCurrentIndex(m_selectedIndices[4]);
    }

    if(ui->m_leftComboBox->count() > m_selectedIndices[5]) {
        ui->m_leftComboBox->setCurrentIndex(m_selectedIndices[5]);
    }
}
