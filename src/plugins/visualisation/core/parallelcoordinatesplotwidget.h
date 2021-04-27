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
#ifndef PARALLELCOORDINATESPLOTWIDGET_H
#define PARALLELCOORDINATESPLOTWIDGET_H
#include <visualisation/core/visualisationwidget.h>

#include <QObject>

class SelectPlotVarsForm;

namespace Visualisation {
class ParallelCoordinatesPlotWidget : public VisualisationWidget
{
    Q_OBJECT

public:
    explicit ParallelCoordinatesPlotWidget(QWidget *parent = 0);
    ~ParallelCoordinatesPlotWidget();
    void initialise();

    SelectPlotVarsForm *varsSelectForm() const;

private slots:
    void updateSelection();

protected:
    void showVariableSelectionForm();

private:
    SelectPlotVarsForm* m_varsSelectForm;
};

}

#endif // PARALLELCOORDINATESPLOTWIDGET_H
