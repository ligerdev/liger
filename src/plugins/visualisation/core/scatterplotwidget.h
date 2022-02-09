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
#ifndef SCATTERPLOTWIDGET_H
#define SCATTERPLOTWIDGET_H
#include <visualisation/core/visualisationwidget.h>

#include <QObject>

class SelectScatterPlotVarsForm;

namespace Visualisation {
class ScatterPlotDataModel;

class ScatterPlotWidget : public VisualisationWidget
{
    Q_OBJECT

public:
    explicit ScatterPlotWidget(QWidget *parent = 0);
    ~ScatterPlotWidget();
    void initialise();
    void setSelectedIndices(const QVariantList& selected);
    void setLabels(const QStringList &labels);

public slots:
    void updateSelectedIndices();
    void updateDisplayVariableOptions();

protected:
    void showVariableSelectionForm();

private:
    ScatterPlotDataModel *data() const;

    SelectScatterPlotVarsForm* m_varsSelectForm;
};

}

#endif // SCATTERPLOTWIDGET_H
