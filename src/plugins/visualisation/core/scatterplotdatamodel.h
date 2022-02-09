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
#ifndef SCATTERPLOTDATAMODEL_H
#define SCATTERPLOTDATAMODEL_H

#include <visualisation/core/visualisationdatamodel.h>

namespace Visualisation {

class ScatterPlotDataModel : public VisualisationDataModel
{
    Q_OBJECT

    Q_PROPERTY(bool displayScatterPlotPointColor
               READ displayScatterPlotPointColor
               WRITE setDisplayScatterPlotPointColor
               NOTIFY displayScatterPlotPointColorChanged)
    Q_PROPERTY(bool displayScatterPlotPointSize
               READ displayScatterPlotPointSize
               WRITE setDisplayScatterPlotPointSize
               NOTIFY displayScatterPlotPointSizeChanged)

public:
    explicit ScatterPlotDataModel(QObject *parent = nullptr);

    bool displayScatterPlotPointColor() const;
    void setDisplayScatterPlotPointColor(bool flag);

    bool displayScatterPlotPointSize() const;
    void setDisplayScatterPlotPointSize(bool flag);

signals:
    void displayScatterPlotPointColorChanged();
    void displayScatterPlotPointSizeChanged();

private:
    bool m_displayScatterPlotPointColor;
    bool m_displayScatterPlotPointSize;
};

} // namespace Visualisation
#endif // SCATTERPLOTDATAMODEL_H
