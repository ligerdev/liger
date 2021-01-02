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
#include <visualisation/qvizparallelcoordinatesnode.h>
#include <visualisation/core/parallelcoordinatesplotwidget.h>
#include <visualisation/core/selectplotvarsform.h>

#include <tigon/Tigon.h>
#include <QAction>
#include <QDebug>

using namespace Visualisation;
using namespace Tigon;

QvizParallelCoordinatesNode::QvizParallelCoordinatesNode()
{
    setSvg(":/visualisation/images/qvizparallelcoordinatesnode.svg");
}

void QvizParallelCoordinatesNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    ParallelCoordinatesPlotWidget* widget = new ParallelCoordinatesPlotWidget;

    initialiseVizNode(widget);

    showWidget();
}

void QvizParallelCoordinatesNode::customiseWidget(VisualisationWidget* widget)
{
    ParallelCoordinatesPlotWidget* pWidget =
            static_cast<ParallelCoordinatesPlotWidget*>(widget);
    connect(pWidget->varsSelectForm(),
            &SelectPlotVarsForm::RobustnessIndicatorsUpdated,
            this, &QvizParallelCoordinatesNode::updateRobustness);
}

QvizParallelCoordinatesNode::~QvizParallelCoordinatesNode()
{

}

void QvizParallelCoordinatesNode::show()
{
    ParallelCoordinatesPlotWidget* widget = new ParallelCoordinatesPlotWidget;

    initialiseVizNode(widget);

    showWidget();
}
