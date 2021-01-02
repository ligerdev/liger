/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <visualisation/core/visualisationdatamodel.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
using namespace Visualisation;

VisualisationDataModel::VisualisationDataModel(QObject *parent)
    : QObject(parent)
    , m_displayPreferences(true)
{

}

/*!
 * \brief Recommanded function for setting data and names.
 */
void VisualisationDataModel::setData(const QVector<QVector<qreal> > &data,
                                     const QStringList &names)
{
    if(data.size() != names.size()) {
        return;
    }
    for(int i=0; i<data.size(); ++i) {
        if(data[i].size() != names.size()) {
            return;
        }
    }
    m_data = data;
    m_names = names;
    generateDataJsonDocument();
    generateScaleJsonDcoument();
    emit dataChanged();
    emit namesChanged();
    emit scaleChanged();
}

void VisualisationDataModel::setData(const QVector<QVector<qreal> > &data)
{
    m_data = data;
    generateDataJsonDocument();
    generateScaleJsonDcoument();
    emit dataChanged();
    emit scaleChanged();
}

void VisualisationDataModel::setNames(const QStringList &names)
{
    for(int i=0; i<m_data.size(); ++i) {
        if(m_data[i].size() != names.size()) {
            return;
        }
    }
    m_names = names;
    generateDataJsonDocument();
    generateScaleJsonDcoument();
    emit namesChanged();
    emit scaleChanged();
}

void VisualisationDataModel::setBoxplotData(const QVector<QVector<QVector<qreal> > > &boxPlotData)
{
    if(boxPlotData.size() != m_data.size()) {
        return;
    }
    m_boxplotData = boxPlotData;
    generateBoxplotJsonDcoument();
    generateScaleJsonDcoument();
    emit boxplotDataChanged();
    emit scaleChanged();
}

QVector<QVector<qreal> > VisualisationDataModel::rawData() const
{
    return m_data;
}

QStringList VisualisationDataModel::allNames() const
{
    return m_names;
}

QJsonDocument VisualisationDataModel::dataJson() const
{
    return m_dataJson;
}

int VisualisationDataModel::dataRows() const
{
    return m_data.size();
}

int VisualisationDataModel::dataCols() const
{
    int size  = 0;
    if(dataRows() > 0) {
        size =  m_data[0].size();
    }
    return size;
}

void VisualisationDataModel::generateDataJsonDocument()
{
    QJsonArray jsonArray;
    for(int i=0; i<m_data.size(); ++i) {
        QJsonObject object;
        for(int j=0; j<m_names.size(); ++j) {
            object[m_names[j]] = m_data[i][j];
        }
        jsonArray.append(object);
    }
    m_dataJson.setArray(jsonArray);
}

void VisualisationDataModel::generateBoxplotJsonDcoument()
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

void VisualisationDataModel::generateScaleJsonDcoument()
{
    /// Error check
    // Do not operate before both data structures are defined
    if(m_data.size() != m_boxplotData.size()) {
        return;
    }

    /// Populate the limits
    int n = m_names.size();
    QVector<qreal> minVec(n, -0.01);
    QVector<qreal> maxVec(n,  0.01);

    if(m_data.size() > 0) {
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
            for(int i=0; i<m_data.size(); ++i) {
                qreal val = m_data[i][j];
                if(val < minVec[j]) {
                    minVec[j] = val;
                } else if(val > maxVec[j]) {
                    maxVec[j] = val;
                }
            }
        }

        if(m_displayPreferences) {
            // update min and max according to goals
            for(int i=0; i<m_goals.size(); ++i) {
                if(!m_goals[i].isNull()) {
                    qreal val = m_goals[i].toReal();
                    if(val < minVec[i]) {
                        minVec[i] = val;
                    } else if(val > maxVec[i]) {
                        maxVec[i] = val;
                    }
                }
            }

            // update min and max according to thresholds
            for(int i=0; i<m_thresholds.size(); ++i) {
                if(!m_thresholds[i].isNull()) {
                    qreal val = m_thresholds[i].toReal();
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
            if(minVec[i] == maxVec[i]) {
                minVec[i] -= 1e-3;
                maxVec[i] += 1e-3;
            }
        }
    }

    ///[] Create Json objects
    QJsonObject minJson, maxJson;
    for(int j=0; j<n; ++j) {
        minJson[m_names[j]] = minVec[j];
        maxJson[m_names[j]] = maxVec[j];
    }

    QJsonArray jsonArray;
    jsonArray.append(minJson);
    jsonArray.append(maxJson);

    m_scaleJson.setArray(jsonArray);
}

QVariantList VisualisationDataModel::preferenceDirerctions() const
{
    return m_preferenceDirerctions;
}

void VisualisationDataModel::setPreferenceDirerctions(const QVariantList &preferenceDirerctions)
{
    m_preferenceDirerctions = preferenceDirerctions;
}

QVariantList VisualisationDataModel::goals() const
{
    return m_goals;
}

void VisualisationDataModel::setGoals(const QVariantList &goals)
{
    m_goals = goals;
    generateScaleJsonDcoument();
    emit scaleChanged();
    emit goalsChanged();
}

QVariantList VisualisationDataModel::thresholds() const
{
    return m_thresholds;
}

void VisualisationDataModel::setThresholds(const QVariantList &thresholds)
{
    m_thresholds = thresholds;
    generateScaleJsonDcoument();
    emit scaleChanged();
    emit thresholdsChanged();
}

bool VisualisationDataModel::displayPreferences() const
{
    return m_displayPreferences;
}

void VisualisationDataModel::setDisplayPreferences(bool disp)
{
    m_displayPreferences = disp;
    generateScaleJsonDcoument();
    emit scaleChanged();
    emit displayPreferencesChanged();
}

QVariantList VisualisationDataModel::colCategories() const
{
    return m_colCategories;
}

void VisualisationDataModel::setColCategories(const QVariantList &colCategores)
{
    m_colCategories = colCategores;
}

QList<int> VisualisationDataModel::brushedIndices() const
{
    return m_brushedIndices;
}

QMap<int, QVector<qreal> > VisualisationDataModel::brushedBounds() const
{
    return m_brushedBounds;
}

void VisualisationDataModel::setBrushedIndices(const QVariantList &brushedIndices)
{
    m_brushedIndices.clear();
    for(int i=0; i<brushedIndices.size(); i++) {
        m_brushedIndices.append(brushedIndices[i].toInt());
    }
    emit brushedIndicesChanged(m_brushedIndices);
}

void VisualisationDataModel::resetBrushedIndices()
{
    m_brushedIndices.clear();
    for(int i=0; i<m_data.size(); i++) {
        m_brushedIndices.append(i);
    }
    emit brushedIndicesChanged(m_brushedIndices);
}

void VisualisationDataModel::setBrushedBounds(const int &column, QVariant lb, QVariant ub)
{
    if(column < 0 || column >= m_names.size()) {
        return;
    }

    m_brushedBounds[column].clear();
    m_brushedBounds[column].append(lb.toReal());
    m_brushedBounds[column].append(ub.toReal());
    emit brushedBoundsChanged(column, lb.toReal(), ub.toReal());
}

QJsonDocument VisualisationDataModel::boxplotDataJson() const
{
    return m_boxplotDataJson;
}

QJsonDocument VisualisationDataModel::scaleJson() const
{
    return m_scaleJson;
}

QVariantList VisualisationDataModel::selectedIndices() const
{
    return m_selectedIndices;
}

void VisualisationDataModel::setSelectedIndices(const QVariantList &selectedIndices)
{
    m_selectedIndices = selectedIndices;
    emit selectedIndicesChanged();
}
