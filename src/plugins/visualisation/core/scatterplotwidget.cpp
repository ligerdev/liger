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
#include <visualisation/core/scatterplotwidget.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <visualisation/core/visualisationviewmodel.h>

#include <QDebug>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

using namespace Visualisation;

ScatterPlotWidget::ScatterPlotWidget(QWidget *parent)
    : VisualisationWidget(parent)
    , m_xcombo(0)
    , m_ycombo(0)
{
    initialise();
}

ScatterPlotWidget::~ScatterPlotWidget()
{
    delete m_xcombo;
    delete m_ycombo;
}

void ScatterPlotWidget::initialise()
{
    VisualisationDataModel* dataModel = new VisualisationDataModel;
    VisualisationViewModel* view = new VisualisationViewModel(this);
    view->setUrl(QUrl("qrc:/visualisation/html/scatterplot.html"));
    view->setDataModel(dataModel);
    view->linkWithJavaScript();
    setView(view);

    m_xcombo = new QComboBox(centralWidget());
    m_ycombo = new QComboBox(centralWidget());

    addItemToToolBar(new QLabel("X: ", centralWidget()));
    addItemToToolBar(m_xcombo);
    addItemToToolBar(new QLabel("Y: ", centralWidget()));
    addItemToToolBar(m_ycombo);

    QPushButton* button = new QPushButton("Refresh", centralWidget());
    addItemToToolBar(button);
    connect(button, SIGNAL(clicked()), this, SLOT(updateSelectedIndices()));

    setWindowTitle(QString("Scatter Plot"));

    connect(data(), SIGNAL(brushedBoundsChanged(int,qreal,qreal)),
            this, SIGNAL(brushedBoundsUpdated(int,qreal,qreal)));
}

void ScatterPlotWidget::setSelectedIndices(const QVariantList &selected)
{
    if(selected.size() < 2) {
        return;
    }
    m_xcombo->setCurrentIndex(selected[0].toInt());
    m_ycombo->setCurrentIndex(selected[1].toInt());
    VisualisationWidget::setSelectedIndices(selected);
}

void ScatterPlotWidget::setXLabels(const QStringList &xlabels)
{
    m_xcombo->clear();
    m_xcombo->insertItems(0, xlabels);
}

void ScatterPlotWidget::setYLabels(const QStringList &ylabels)
{
    m_ycombo->clear();
    m_ycombo->insertItems(0, ylabels);
}

void ScatterPlotWidget::updateSelectedIndices()
{
    QVariantList selected;
    selected.append(m_xcombo->currentIndex());
    selected.append(m_ycombo->currentIndex());
    VisualisationWidget::setSelectedIndices(selected);
}

void ScatterPlotWidget::showVariableSelectionForm()
{
    this->raise();
    setXLabels(data()->allNames());
    setYLabels(data()->allNames());
    m_xcombo->showPopup();
}
