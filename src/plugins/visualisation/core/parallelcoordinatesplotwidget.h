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
#ifndef PARALLELCOORDINATESPLOTWIDGET_H
#define PARALLELCOORDINATESPLOTWIDGET_H
#include <visualisation/core/visualisationwidget.h>

#include <QObject>

class SelectPlotVarsForm;

namespace Visualisation {
class ParallelCoordinatesPlotDataModel;

class ParallelCoordinatesPlotWidget : public VisualisationWidget
{
    Q_OBJECT

public:
    explicit ParallelCoordinatesPlotWidget(QWidget *parent = 0);
    ~ParallelCoordinatesPlotWidget();
    void initialise();

    SelectPlotVarsForm *varsSelectForm() const;

    void setBoxPlotData(const QVector<QVector<QVector<qreal>>> &boxplotData);

signals:
    void setDisplayPreferences(bool disp);

private slots:
    void updateSelection();

protected:
    void showVariableSelectionForm();

private:
    ParallelCoordinatesPlotDataModel *data() const;
    void setView(VisualisationViewModel *view);

    SelectPlotVarsForm* m_varsSelectForm;
};

}

#endif // PARALLELCOORDINATESPLOTWIDGET_H
