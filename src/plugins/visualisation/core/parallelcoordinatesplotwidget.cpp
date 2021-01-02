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
#include <visualisation/core/parallelcoordinatesplotwidget.h>
#include <visualisation/core/visualisationviewmodel.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <visualisation/visualisationconstants.h>
#include <visualisation/core/selectplotvarsform.h>

#include <QGridLayout>
#include <QPushButton>
#include <QDebug>

using namespace Visualisation;

ParallelCoordinatesPlotWidget::ParallelCoordinatesPlotWidget(QWidget *parent)
    : VisualisationWidget(parent)
    , m_varsSelectForm(new SelectPlotVarsForm(this))
{
    initialise();
}

ParallelCoordinatesPlotWidget::~ParallelCoordinatesPlotWidget()
{
    delete m_varsSelectForm;
}

void ParallelCoordinatesPlotWidget::initialise()
{
    VisualisationDataModel* dataModel = new VisualisationDataModel;
    VisualisationViewModel* view = new VisualisationViewModel(this);
    view->setUrl(QUrl("qrc:/visualisation/html/parallelcoordinatesplot.html"));
    view->setDataModel(dataModel);
    view->linkWithJavaScript();
    setView(view);
    // Add select variable form
    QPushButton* button = new QPushButton("Select Variables", centralWidget());
    connect(button, &QPushButton::clicked, this,
            &ParallelCoordinatesPlotWidget::showVariableSelectionForm);
    addItemToToolBar(button);

    setWindowTitle(QString("Parallel Coordinates Plot"));
    connect(m_varsSelectForm, SIGNAL(VariablesUpdated()),
            this, SLOT(updateSelection()));
    connect(data(), SIGNAL(brushedBoundsChanged(int,qreal,qreal)),
            this, SIGNAL(brushedBoundsUpdated(int,qreal,qreal)));
}

void ParallelCoordinatesPlotWidget::updateSelection()
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

void ParallelCoordinatesPlotWidget::showVariableSelectionForm()
{
    m_varsSelectForm->setAllNames(data()->allNames(),
                                  data()->colCategories(),
                                  data()->selectedIndices());
    m_varsSelectForm->show();
    m_varsSelectForm->raise();
}

SelectPlotVarsForm *ParallelCoordinatesPlotWidget::varsSelectForm() const
{
    return m_varsSelectForm;
}
