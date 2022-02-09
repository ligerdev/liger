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
#include <visualisation/core/scatterplotwidget.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <visualisation/core/scatterplotdatamodel.h>
#include <visualisation/core/visualisationviewmodel.h>
#include <visualisation/core/selectscatterplotvarsform.h>

namespace Visualisation {

ScatterPlotWidget::ScatterPlotWidget(QWidget *parent)
    : VisualisationWidget(parent)
    , m_varsSelectForm(new SelectScatterPlotVarsForm(this))
{
    initialise();
}

ScatterPlotWidget::~ScatterPlotWidget()
{
    delete m_varsSelectForm;
}

void ScatterPlotWidget::initialise()
{
    ScatterPlotDataModel* dataModel = new ScatterPlotDataModel;
    VisualisationViewModel* view = new VisualisationViewModel(this);
    view->setUrl(QUrl("qrc:/visualisation/html/scatterplot.html"));
    view->setDataModel(dataModel);
    view->linkWithJavaScript();
    setView(view);

    QPushButton* varButton = new QPushButton("Select Variables", centralWidget());
    connect(varButton, &QPushButton::clicked, this,
            &ScatterPlotWidget::showVariableSelectionForm);
    addItemToToolBar(varButton);

    QPushButton* button = new QPushButton("Refresh", centralWidget());
    addItemToToolBar(button);
    connect(button, SIGNAL(clicked()), this, SLOT(updateSelectedIndices()));
    connect(button, SIGNAL(clicked()), this, SLOT(updateDisplayVariableOptions()));

    setWindowTitle(QString("Scatter Plot"));

    connect(data(), SIGNAL(brushedBoundsChanged(int,qreal,qreal)),
            this, SIGNAL(brushedBoundsUpdated(int,qreal,qreal)));
}

void ScatterPlotWidget::setSelectedIndices(const QVariantList &selected)
{
    QVariantList sel = selected;
    if(sel.size() < 4) {
        QVariant last = selected.last();
        while(sel.size() < 4) {
            sel.append(last);
        }
    }

    m_varsSelectForm->setSelectedIndices(sel);
    VisualisationWidget::setSelectedIndices(sel);
}

void ScatterPlotWidget::setLabels(const QStringList &labels)
{
    m_varsSelectForm->setXNames(labels);
    m_varsSelectForm->setYNames(labels);
    m_varsSelectForm->setSizeNames(labels);
    m_varsSelectForm->setColorNames(labels);
}

void ScatterPlotWidget::updateSelectedIndices()
{
    QVariantList selected = m_varsSelectForm->selectedIndex();
    m_varsSelectForm->setSelectedIndices(selected);
    VisualisationWidget::setSelectedIndices(selected);
}

void ScatterPlotWidget::updateDisplayVariableOptions()
{
    bool showSize = m_varsSelectForm->showSize();
    bool showColor = m_varsSelectForm->showColor();

    data()->setDisplayScatterPlotPointSize(showSize);
    data()->setDisplayScatterPlotPointColor(showColor);
}


void ScatterPlotWidget::showVariableSelectionForm()
{
    QStringList names = data()->allNames();
    bool displayScatterPlotPointColor = data()->displayScatterPlotPointColor();
    bool displayScatterPlotPointSize = data()->displayScatterPlotPointSize();

    m_varsSelectForm->setXNames(names);
    m_varsSelectForm->setYNames(names);
    m_varsSelectForm->setSizeNames(names);
    m_varsSelectForm->setColorNames(names);

    m_varsSelectForm->setShowColor(displayScatterPlotPointColor);
    m_varsSelectForm->setShowSize(displayScatterPlotPointSize);

    m_varsSelectForm->show();
    m_varsSelectForm->raise();
}

ScatterPlotDataModel* ScatterPlotWidget::data() const
{
    return static_cast<ScatterPlotDataModel*>(VisualisationWidget::data());
}

} // namespace Visualisation
