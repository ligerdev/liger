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
#include <visualisation/core/objectiverankwidget.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <visualisation/core/objectiverankdatamodel.h>
#include <visualisation/core/visualisationviewmodel.h>

namespace Visualisation {

ObjectiveRankWidget::ObjectiveRankWidget(QWidget *parent)
    : VisualisationWidget(parent)
{
    initialise();
}

ObjectiveRankWidget::~ObjectiveRankWidget()
{

}

void ObjectiveRankWidget::initialise()
{
    ObjectiveRankDataModel* dataModel = new ObjectiveRankDataModel;
    VisualisationViewModel* view = new VisualisationViewModel(this);
    view->setUrl(QUrl("qrc:/visualisation/html/barplot.html"));
    view->setDataModel(dataModel);
    view->linkWithJavaScript();
    setView(view);

    QPushButton* runObjRed = new QPushButton("Update", centralWidget());
    addItemToToolBar(runObjRed);

    connect(runObjRed, &QPushButton::clicked, this,
            &ObjectiveRankWidget::runObjectiveReductionAlgorithm);

    setWindowTitle(QString("PCA based Objective Reduction"));
}

void ObjectiveRankWidget::setObjReductionData(
        const QVector<bool>& essentialObjectiveSet,
        const QVector<qreal>& errorPerObjective,
        const QVector<qreal>& variancePerObjective,
        const QVector<qreal>& variancePerPCA,
        const QMap<int, QVector<int>> correlatedObjectivesIdx,
        const QMap<int, QVector<double>> correlatedObjectivesStrength,
        const QStringList& objectiveNames)
{
    static_cast<ObjectiveRankDataModel*>(data())->
            setObjReductionData(essentialObjectiveSet, errorPerObjective,
                                variancePerObjective, variancePerPCA,
                                correlatedObjectivesIdx,
                                correlatedObjectivesStrength, objectiveNames);
}

void ObjectiveRankWidget::setVisualisationType(int visualisationType)
{
    static_cast<ObjectiveRankDataModel*>(data())->
            setVisualisationType(visualisationType);
}

} // namespace Visualisation
