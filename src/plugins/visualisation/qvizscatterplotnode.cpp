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
#include <visualisation/qvizscatterplotnode.h>
#include <visualisation/core/scatterplotwidget.h>

#include <tigon/Tigon.h>

#include <QString>
#include <QAction>

#include <QDebug>
using namespace Visualisation;
using namespace Tigon::Representation;

QVizScatterPlotNode::QVizScatterPlotNode()
{
    setSvg(":/visualisation/images/qvizscatterplotnode.svg");
}

void QVizScatterPlotNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    ScatterPlotWidget* widget = new ScatterPlotWidget;

    initialiseVizNode(widget);

    showWidget();
}

void QVizScatterPlotNode::customiseWidget(VisualisationWidget* widget)
{
    ScatterPlotWidget* sWidget = static_cast<ScatterPlotWidget*>(widget);
    sWidget->setXLabels(sWidget->allNames());
    sWidget->setYLabels(sWidget->allNames());
}

QVizScatterPlotNode::~QVizScatterPlotNode()
{
}
