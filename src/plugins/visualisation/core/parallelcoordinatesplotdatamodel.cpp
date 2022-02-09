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
#include <visualisation/core/parallelcoordinatesplotdatamodel.h>

#include <cmath>
#include <QJsonObject>
#include <QJsonArray>

namespace Visualisation {

ParallelCoordinatesPlotDataModel::ParallelCoordinatesPlotDataModel(QObject *parent)
    : VisualisationDataModel(parent)
    , m_displayPreferences(true)
{

}

void ParallelCoordinatesPlotDataModel::setData(
        const QVector<QVector<qreal>> &data,
        const QStringList &names)
{
    VisualisationDataModel::setData(data, names);
    generateScaleJsonDcoument();
    emit scaleChanged();
}

void ParallelCoordinatesPlotDataModel::setData(
        const QVector<QVector<qreal>> &data)
{
    VisualisationDataModel::setData(data);
    generateScaleJsonDcoument();
    emit scaleChanged();
}

void ParallelCoordinatesPlotDataModel::setNames(const QStringList &names)
{
    VisualisationDataModel::setNames(names);
    generateScaleJsonDcoument();
    emit scaleChanged();
}

void ParallelCoordinatesPlotDataModel::setGoals(const QVariantList &goals)
{
    VisualisationDataModel::setGoals(goals);
    generateScaleJsonDcoument();
    emit scaleChanged();
}

void ParallelCoordinatesPlotDataModel::setThresholds(const QVariantList &thresholds)
{
    VisualisationDataModel::setThresholds(thresholds);
    generateScaleJsonDcoument();
    emit scaleChanged();
}

void ParallelCoordinatesPlotDataModel::setBoxplotData(
        const QVector<QVector<QVector<qreal>>> &boxPlotData)
{
    if(boxPlotData.size() != rawData().size()) {
        return;
    }
    m_boxplotData = boxPlotData;
    generateBoxplotJsonDcoument();
    generateScaleJsonDcoument();
    emit boxplotDataChanged();
    emit scaleChanged();
}

bool ParallelCoordinatesPlotDataModel::displayPreferences() const
{
    return m_displayPreferences;
}

void ParallelCoordinatesPlotDataModel::setDisplayPreferences(bool disp)
{
    m_displayPreferences = disp;
    generateScaleJsonDcoument();
    emit scaleChanged();
    emit displayPreferencesChanged();
}

QJsonDocument ParallelCoordinatesPlotDataModel::boxplotDataJson() const
{
    return m_boxplotDataJson;
}

QJsonDocument ParallelCoordinatesPlotDataModel::scaleJson() const
{
    return m_scaleJson;
}

void ParallelCoordinatesPlotDataModel::generateBoxplotJsonDcoument()
{
    QJsonArray jsonArray;
    for(int i=0; i<m_boxplotData.size(); ++i) {
        QJsonArray jsonInnerArray;
        for(int j=0; j<m_boxplotData[i].size(); ++j) {
            QJsonObject object;
            object["whiskersBottom"]=m_boxplotData[i][j][0];
            object["firstQuartile"] =m_boxplotData[i][j][1];
            object["secondQuartile"]=m_boxplotData[i][j][2];
            object["thirdQuartile"] =m_boxplotData[i][j][3];
            object["whiskersTop"]   =m_boxplotData[i][j][4];
            jsonInnerArray.append(object);
        }
        jsonArray.append(jsonInnerArray);
    }
    m_boxplotDataJson.setArray(jsonArray);
}

void ParallelCoordinatesPlotDataModel::generateScaleJsonDcoument()
{
    /// Error check
    // Do not operate before both data structures are defined
    if(rawData().size() != m_boxplotData.size()) {
        return;
    }

    /// Populate the limits
    int n = allNames().size();
    QVector<qreal> minVec(n, -0.01);
    QVector<qreal> maxVec(n,  0.01);

    if(rawData().size() > 0) {
        // Populate the scale
        for(int j=0; j<n; ++j) {
            // populate min and max with first member's boxplot data
            minVec[j] = m_boxplotData[0][j][0];
            maxVec[j] = m_boxplotData[0][j][4];

            // update min and max according to boxplot data
            for(int i=1; i<m_boxplotData.size(); ++i) {
                if(m_boxplotData[i][j][0] < minVec[j]) {
                    minVec[j] = m_boxplotData[i][j][0];
                }
                if(m_boxplotData[i][j][4] > maxVec[j]) {
                    maxVec[j] = m_boxplotData[i][j][4];
                }
            }

            // update min and max according to data
            for(const QVector<qreal>& vec : rawData()) {
                qreal val = vec[j];
                if(val < minVec[j]) {
                    minVec[j] = val;
                } else if(val > maxVec[j]) {
                    maxVec[j] = val;
                }
            }
        }

        QVector<bool> isThereOnePoint(n, false);
        for(int i=0; i<n; i++) {
            if(minVec[i] == maxVec[i]) {
                isThereOnePoint[i] = true;
            }
        }

        if(m_displayPreferences) {
            // update min and max according to goals
            for(int i=0; i<goals().size(); ++i) {
                if(!goals()[i].isNull()) {
                    qreal val = goals()[i].toReal();
                    if(val < minVec[i]) {
                        minVec[i] = val;
                    } else if(val > maxVec[i]) {
                        maxVec[i] = val;
                    }
                }
            }

            // update min and max according to thresholds
            for(int i=0; i<thresholds().size(); ++i) {
                if(!thresholds()[i].isNull()) {
                    qreal val = thresholds()[i].toReal();
                    if(val < minVec[i]) {
                        minVec[i] = val;
                    } else if(val > maxVec[i]) {
                        maxVec[i] = val;
                    }
                }
            }
        }

        // account for zero scale
        for(int i = 0; i < n; ++i) {
            if(isThereOnePoint[i]) {
                minVec[i] -= 1;
                maxVec[i] += 1;
                minVec[i] = std::floor(minVec[i]);
                maxVec[i] = std::ceil(maxVec[i]);
            }
        }

    }

    ///[] Create Json objects
    QJsonObject minJson, maxJson;
    for(int j=0; j<n; ++j) {
        minJson[allNames()[j]] = minVec[j];
        maxJson[allNames()[j]] = maxVec[j];
    }

    QJsonArray jsonArray;
    jsonArray.append(minJson);
    jsonArray.append(maxJson);

    m_scaleJson.setArray(jsonArray);
}

} // namespace Visualisation
