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
#ifndef OBJECTIVERANKDATAMODEL_H
#define OBJECTIVERANKDATAMODEL_H

#include <visualisation/core/visualisationdatamodel.h>

namespace Visualisation {

class ObjectiveRankDataModel : public VisualisationDataModel
{
    Q_OBJECT

    Q_PROPERTY(QJsonDocument objReductionDataJson
               MEMBER m_objReductionDataJson
               READ objReductionDataJson
               NOTIFY objReductionDataChanged)

    Q_PROPERTY(int visualisationType
               READ visualisationType
               WRITE setVisualisationType
               NOTIFY visualisationTypeChanged)

public:
    explicit ObjectiveRankDataModel(QObject *parent = nullptr);

    void setObjReductionData(
            const QVector<bool>& essentialObjectiveSet,
            const QVector<qreal>& errorPerObjective,
            const QVector<qreal>& variancePerObjective,
            const QVector<qreal>& variancePerPCA,
            const QMap<int, QVector<int>> correlatedObjectivesIdx,
            const QMap<int, QVector<double>> correlatedObjectivesStrength,
            const QStringList& objectiveNames);
    QJsonDocument objReductionDataJson() const;

    void setVisualisationType(int visualisationType);
    int visualisationType() const;

signals:
    void objReductionDataChanged();
    void visualisationTypeChanged();

private:
    void generateObjReductionJsonDocument();

    QJsonDocument m_objReductionDataJson;
    QVector<bool> m_essentialObjectiveSet;
    QVector<qreal> m_errorPerObjective;
    QVector<qreal> m_variancePerObjective;
    QVector<qreal> m_variancePerPCA;
    QMap<int, QVector<int>> m_correlatedObjectivesIdx;
    QMap<int, QVector<double>> m_correlatedObjectivesStrength;
    QStringList m_objectiveNames;

    int m_visualisationType;
};

}
#endif // OBJECTIVERANKDATAMODEL_H
