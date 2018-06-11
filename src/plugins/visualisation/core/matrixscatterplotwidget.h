/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef MATRIXSCATTERPLOTWIDGET_H
#define MATRIXSCATTERPLOTWIDGET_H
#include <visualisation/core/visualisationwidget.h>
#include <visualisation/core/selectplotvarsform.h>

#include <QPushButton>
#include <QObject>

namespace Visualisation {

class MatrixScatterPlotWidget : public VisualisationWidget
{
    Q_OBJECT

public:
    explicit MatrixScatterPlotWidget(QWidget *parent = 0);
    ~MatrixScatterPlotWidget();
    void initialise();

private slots:
    void updateSelection();

protected:
    void showVariableSelectionForm();

private:
    SelectPlotVarsForm* m_varsSelectForm;
};

}
#endif // MATRIXSCATTERPLOTWIDGET_H
