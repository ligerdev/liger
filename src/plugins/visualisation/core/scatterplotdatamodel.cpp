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
#include <visualisation/core/scatterplotdatamodel.h>

namespace Visualisation {

ScatterPlotDataModel::ScatterPlotDataModel(QObject *parent)
    : VisualisationDataModel(parent)
    , m_displayScatterPlotPointColor(false)
    , m_displayScatterPlotPointSize(false)
{

}

bool ScatterPlotDataModel::displayScatterPlotPointColor() const
{
    return m_displayScatterPlotPointColor;
}

void ScatterPlotDataModel::setDisplayScatterPlotPointColor(bool flag)
{
    m_displayScatterPlotPointColor = flag;
    emit displayScatterPlotPointColorChanged();
}

bool ScatterPlotDataModel::displayScatterPlotPointSize() const
{
    return m_displayScatterPlotPointSize;
}

void ScatterPlotDataModel::setDisplayScatterPlotPointSize(bool flag)
{
    m_displayScatterPlotPointSize = flag;
    emit displayScatterPlotPointSizeChanged();
}

} // namespace Visualisation
