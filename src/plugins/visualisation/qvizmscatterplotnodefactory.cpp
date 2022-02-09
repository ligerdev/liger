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
#include <visualisation/qvizmscatterplotnodefactory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>
#include <designer/iengine/iprocessnode.h>

using namespace Visualisation;

QVizMScatterPlotNodeFactory::QVizMScatterPlotNodeFactory()
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();
    m_toolBoxButton = dtoolBox->registerVisualization(this,
                                                      QString("Matrix ScatterPlot visualization node"),
                                                      QString(":/visualisation/images/qvizmscatterplotnode.svg"));
}

Designer::IProcessNode* QVizMScatterPlotNodeFactory::createProcessNode()
{
    return new QVizMScatterPlotNode();
}

bool QVizMScatterPlotNodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QVizMScatterPlotNodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}
