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
#include <visualisation/qvizglyphsplotnode.h>
#include <visualisation/core/glyphsplotwidget.h>

namespace Visualisation {

QVizGlyphsPlotNode::QVizGlyphsPlotNode()
{
    setSvg(":/visualisation/images/qvizglyphsplot.svg");
}

void QVizGlyphsPlotNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    GlyphsPlotWidget* widget = new GlyphsPlotWidget;

    initialiseVizNode(widget);

    showWidget();
}

void QVizGlyphsPlotNode::customiseWidget(VisualisationWidget* widget)
{
    GlyphsPlotWidget* sWidget = static_cast<GlyphsPlotWidget*>(widget);
    sWidget->setLabels(sWidget->allNames());

    connect(sWidget, &VisualisationWidget::brushedBoundsUpdated,
            this, &QVizGlyphsPlotNode::receivedBrushedBounds);

    setupSaveBrushedSolutionsFileOption();
    setupSaveAlllSolutionsFileOption();
    setupSelectVariablesEditOptions();

    setupZoomToBrushedButton();

    connect(sWidget, &VisualisationWidget::brushedIndicesUpdated,
            this, &QVizGlyphsPlotNode::resetBrushedButton);

    sWidget->addSpacerToToolBar();

    QString checkboxStyle = "QCheckBox {color: white}";
    setupDominatedCheckbox(checkboxStyle);
    setupInfeasibleCheckbox(checkboxStyle);
    setupNonPertientCheckbox(checkboxStyle);
    setupRecordGoalsCheckbox(checkboxStyle);
    setupInSyncCheckbox(checkboxStyle);

    sWidget->addSpacerToToolBar();
    setupSetSelection();
    sWidget->addSpacerToToolBar();
    setupTimedTracking();
    sWidget->addSpacerToToolBar();
    setupIterationTracking();
    sWidget->addSpacerToToolBar();
    setupIterationSelection();
}

QVizGlyphsPlotNode::~QVizGlyphsPlotNode()
{

}

} // namespace Visualisation
