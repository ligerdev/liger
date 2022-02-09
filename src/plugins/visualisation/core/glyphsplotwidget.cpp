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
#include <visualisation/core/glyphsplotwidget.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <visualisation/core/glyphsplotdatamodel.h>
#include <visualisation/core/visualisationviewmodel.h>
#include <visualisation/core/selectglyphsplotvarsform.h>

namespace Visualisation {

GlyphsPlotWidget::GlyphsPlotWidget(QWidget *parent)
    : VisualisationWidget(parent)
    , m_varsSelectForm(new SelectGlyphsPlotVarsForm(this))
{
    initialise();
}

GlyphsPlotWidget::~GlyphsPlotWidget()
{
    delete m_varsSelectForm;
}

void GlyphsPlotWidget::initialise()
{
    GlyphsPlotDataModel* dataModel = new GlyphsPlotDataModel;
    VisualisationViewModel* view = new VisualisationViewModel(this);
    view->setUrl(QUrl("qrc:/visualisation/html/glyphsplot.html"));
    view->setDataModel(dataModel);
    view->linkWithJavaScript();
    setView(view);

    QPushButton* varButton = new QPushButton("Select Variables", centralWidget());
    connect(varButton, &QPushButton::clicked, this,
            &GlyphsPlotWidget::showVariableSelectionForm);
    addItemToToolBar(varButton);

    QPushButton* button = new QPushButton("Refresh", centralWidget());
    addItemToToolBar(button);
    connect(button, SIGNAL(clicked()), this, SLOT(updateSelectedIndices()));

    setWindowTitle(QString("Glyphs Plot"));

    connect(data(), SIGNAL(brushedBoundsChanged(int,qreal,qreal)),
            this, SIGNAL(brushedBoundsUpdated(int,qreal,qreal)));
}

void GlyphsPlotWidget::setSelectedIndices(const QVariantList &selected)
{
    QVariantList sel = selected;
    if(sel.size() < 6) {
        QVariant last = selected.last();
        while(sel.size() < 6) {
            sel.append(last);
        }
    }

    m_varsSelectForm->setSelectedIndices(sel);
    VisualisationWidget::setSelectedIndices(sel);
}

void GlyphsPlotWidget::setLabels(const QStringList &labels)
{
    m_varsSelectForm->setXNames(labels);
    m_varsSelectForm->setYNames(labels);
    m_varsSelectForm->setTopNames(labels);
    m_varsSelectForm->setBottomNames(labels);
    m_varsSelectForm->setRightNames(labels);
    m_varsSelectForm->setLeftNames(labels);
}

void GlyphsPlotWidget::updateSelectedIndices()
{
    QVariantList selected = m_varsSelectForm->selectedIndex();
    m_varsSelectForm->setSelectedIndices(selected);
    VisualisationWidget::setSelectedIndices(selected);
}

void GlyphsPlotWidget::showVariableSelectionForm()
{
    QStringList names = data()->allNames();

    m_varsSelectForm->setXNames(names);
    m_varsSelectForm->setYNames(names);
    m_varsSelectForm->setTopNames(names);
    m_varsSelectForm->setBottomNames(names);
    m_varsSelectForm->setRightNames(names);
    m_varsSelectForm->setLeftNames(names);

    m_varsSelectForm->show();
    m_varsSelectForm->raise();
}

} // namespace Visualisation
