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
#include <visualisation/core/objectiverankdatamodel.h>

#include <QJsonObject>
#include <QJsonArray>

namespace Visualisation {

ObjectiveRankDataModel::ObjectiveRankDataModel(QObject *parent)
    : VisualisationDataModel(parent)
    , m_essentialObjectiveSet(QVector<bool>{true})
    , m_errorPerObjective(QVector<qreal>{1.0})
{
    m_correlatedObjectivesIdx[0] = QVector<int>();
    m_correlatedObjectivesStrength[0] = QVector<double>();
    m_objectiveNames << "f";
}

void ObjectiveRankDataModel::setObjReductionData(
        const QVector<bool>& essentialObjectiveSet,
        const QVector<qreal>& errorPerObjective,
        const QVector<qreal>& variancePerObjective,
        const QVector<qreal>& variancePerPCA,
        const QMap<int, QVector<int>> correlatedObjectivesIdx,
        const QMap<int, QVector<double>> correlatedObjectivesStrength,
        const QStringList& objectiveNames)
{
    if(essentialObjectiveSet.size() < 1) {
        return;
    }
    if(essentialObjectiveSet.size() != errorPerObjective.size()) {
        return;
    }
    if(essentialObjectiveSet.size() != variancePerObjective.size()) {
        return;
    }
    if(essentialObjectiveSet.size() != variancePerPCA.size()) {
        return;
    }
    if(essentialObjectiveSet.size() != correlatedObjectivesIdx.size()) {
        return;
    }
    if(essentialObjectiveSet.size() != correlatedObjectivesStrength.size()) {
        return;
    }
    if(essentialObjectiveSet.size() != objectiveNames.size()) {
        return;
    }

    m_essentialObjectiveSet = essentialObjectiveSet;
    m_errorPerObjective = errorPerObjective;
    m_variancePerObjective = variancePerObjective;
    m_variancePerPCA = variancePerPCA;
    m_correlatedObjectivesIdx = correlatedObjectivesIdx;
    m_correlatedObjectivesStrength = correlatedObjectivesStrength;
    m_objectiveNames = objectiveNames;

    generateObjReductionJsonDocument();
    emit objReductionDataChanged();
}

void ObjectiveRankDataModel::generateObjReductionJsonDocument()
{
    QJsonArray jsonArray;
    for(int i=0; i<m_objectiveNames.size(); i++) {
        QJsonObject object;
        object["name"] = m_objectiveNames[i];
        object["isObjectiveEssential"] = m_essentialObjectiveSet[i];
        object["error"] = m_errorPerObjective[i];
        object["variancePerObjective"] = m_variancePerObjective[i];
        object["variancePerPCA"] = m_variancePerPCA[i];
        QVector<int> corObjsIdx = m_correlatedObjectivesIdx[i];
        QVector<double> corObjsStrength = m_correlatedObjectivesStrength[i];
        QJsonArray corObjsArray;
        for(int j=0; j<corObjsIdx.size(); j++) {
            QJsonObject corObjsObject;
            corObjsObject["name"] = m_objectiveNames[corObjsIdx[j]];
            corObjsObject["strength"] = corObjsStrength[j];
            corObjsArray.push_back(corObjsObject);
        }
        object["correlatedObjectives"] = corObjsArray;
        jsonArray.append(object);
    }

    m_objReductionDataJson.setArray(jsonArray);
}

QJsonDocument ObjectiveRankDataModel::objReductionDataJson() const
{
    return m_objReductionDataJson;
}

void ObjectiveRankDataModel::setVisualisationType(int visualisationType)
{
    m_visualisationType = visualisationType;
    emit visualisationTypeChanged();
}

int ObjectiveRankDataModel::visualisationType() const
{
    return m_visualisationType;
}

} // namespace Visualisation
