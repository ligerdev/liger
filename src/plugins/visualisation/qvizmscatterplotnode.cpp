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
#include <visualisation/qvizmscatterplotnode.h>
#include <visualisation/core/matrixscatterplotwidget.h>
#include <tigon/Tigon.h>
#include <QDebug>

using namespace Visualisation;
using namespace Tigon::Representation;

QVizMScatterPlotNode::QVizMScatterPlotNode()
{
    setSvg(":/visualisation/images/qvizmscatterplotnode.svg");
}

void QVizMScatterPlotNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    MatrixScatterPlotWidget* widget = new MatrixScatterPlotWidget;

    initialiseVizNode(widget);

    showWidget();
}

void QVizMScatterPlotNode::customiseWidget(VisualisationWidget* widget)
{
    Q_UNUSED(widget);
}

QVizMScatterPlotNode::~QVizMScatterPlotNode()
{
}
