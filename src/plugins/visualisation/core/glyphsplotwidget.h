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
#ifndef GLYPHSPLOTWIDGET_H
#define GLYPHSPLOTWIDGET_H
#include <visualisation/core/visualisationwidget.h>

#include <QObject>

class SelectGlyphsPlotVarsForm;

namespace Visualisation {

class GlyphsPlotWidget : public VisualisationWidget
{
    Q_OBJECT

public:
    explicit GlyphsPlotWidget(QWidget *parent = 0);
    ~GlyphsPlotWidget();
    void initialise();
    void setSelectedIndices(const QVariantList& selected);
    void setLabels(const QStringList &labels);

public slots:
    void updateSelectedIndices();

protected:
    void showVariableSelectionForm();

private:
    SelectGlyphsPlotVarsForm* m_varsSelectForm;
};

} // namespace Visualisation

#endif // GLYPHSPLOTWIDGET_H
