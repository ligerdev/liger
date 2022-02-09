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
#include <visualisation/core/matrixscatterplotwidget.h>
#include <visualisation/core/visualisationviewmodel.h>
#include <visualisation/core/visualisationdatamodel.h>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>

using namespace Visualisation;

MatrixScatterPlotWidget::MatrixScatterPlotWidget(QWidget *parent)
    : VisualisationWidget(parent)
    , m_varsSelectForm(new SelectPlotVarsForm(this))
{
    initialise();
}

MatrixScatterPlotWidget::~MatrixScatterPlotWidget()
{
    delete m_varsSelectForm;
}

void MatrixScatterPlotWidget::initialise()
{
    VisualisationDataModel* dataModel = new VisualisationDataModel;
    VisualisationViewModel* view = new VisualisationViewModel(this);
    view->setUrl(QUrl("qrc:/visualisation/html/matrixscatterplot.html"));
    view->setDataModel(dataModel);
    view->linkWithJavaScript();
    setView(view);
    // Add select variable form
    QPushButton* button = new QPushButton("Select Variables", centralWidget());
    connect(button, &QPushButton::clicked, this,
            &MatrixScatterPlotWidget::showVariableSelectionForm);
    addItemToToolBar(button);
    setWindowTitle(QString("Matrix Scatter Plot"));
    connect(m_varsSelectForm, &SelectPlotVarsForm::VariablesUpdated,
            this, &MatrixScatterPlotWidget::updateSelection);
}

void MatrixScatterPlotWidget::updateSelection()
{
    QStringList selectedVars = m_varsSelectForm->selectedVars();
    QStringList allNames = data()->allNames();
    QVariantList indices;
    foreach(const QString& it, selectedVars) {
        foreach(const QString& st, allNames) {
            if(it==st) {
                int cindex = allNames.indexOf(st);
                indices.append(QVariant::fromValue(cindex));
                break;
            }
        }
    }
    setSelectedIndices(indices);
}

void MatrixScatterPlotWidget::showVariableSelectionForm()
{
    m_varsSelectForm->setAllNames(data()->allNames(),
                                  data()->colCategories(),
                                  data()->selectedIndices());
    m_varsSelectForm->show();
    m_varsSelectForm->raise();
}
