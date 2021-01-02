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
#include <visualisation/qvizscatterplotnodefactory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>
#include <designer/iengine/iprocessnode.h>

using namespace Visualisation;

QVizScatterPlotNodeFactory::QVizScatterPlotNodeFactory()
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();
    m_toolBoxButton = dtoolBox->registerVisualization(this, QString("ScatterPlot visualization node."),
                      QString(":/visualisation/images/qvizscatterplotnode.svg"));
}

Designer::IProcessNode* QVizScatterPlotNodeFactory::createProcessNode()
{
    return new QVizScatterPlotNode();
}

bool QVizScatterPlotNodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QVizScatterPlotNodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}
