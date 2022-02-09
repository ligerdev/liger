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
#include <visualisation/core/visualisationdatamodel.h>

#include <cmath>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

namespace Visualisation {

VisualisationDataModel::VisualisationDataModel(QObject *parent)
    : QObject(parent)
{

}

void VisualisationDataModel::setData(const QVector<QVector<qreal>> &data,
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
    emit dataChanged();
    emit namesChanged();
}

void VisualisationDataModel::setData(const QVector<QVector<qreal>> &data)
{
    m_data = data;
    generateDataJsonDocument();
    emit dataChanged();
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
    emit namesChanged();
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
    return rawData().size();
}

int VisualisationDataModel::dataCols() const
{
    int size  = 0;
    if(m_data.size() > 0) {
        size =  m_data[0].size();
    }
    return size;
}

QVariantList VisualisationDataModel::colCategories() const
{
    return m_colCategories;
}

void VisualisationDataModel::setColCategories(const QVariantList &colCategores)
{
    m_colCategories = colCategores;
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
    emit goalsChanged();
}

QVariantList VisualisationDataModel::thresholds() const
{
    return m_thresholds;
}

void VisualisationDataModel::setThresholds(const QVariantList &thresholds)
{
    m_thresholds = thresholds;
    emit thresholdsChanged();
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

QVariantList VisualisationDataModel::selectedIndices() const
{
    return m_selectedIndices;
}

void VisualisationDataModel::setSelectedIndices(const QVariantList &selectedIndices)
{
    m_selectedIndices = selectedIndices;
    emit selectedIndicesChanged();
}

} // namespace Visualisation
