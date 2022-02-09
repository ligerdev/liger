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
#ifndef PARALLELCOORDINATESPLOTDATAMODEL_H
#define PARALLELCOORDINATESPLOTDATAMODEL_H

#include <visualisation/core/visualisationdatamodel.h>

namespace Visualisation {

class ParallelCoordinatesPlotDataModel : public VisualisationDataModel
{
    Q_OBJECT

    Q_PROPERTY(QJsonDocument boxplotDataJson MEMBER m_boxplotDataJson
               READ boxplotDataJson NOTIFY boxplotDataChanged)
    Q_PROPERTY(QJsonDocument scaleJson MEMBER m_scaleJson
               READ scaleJson NOTIFY scaleChanged)
    Q_PROPERTY(bool displayPreferences READ displayPreferences
               WRITE setDisplayPreferences NOTIFY displayPreferencesChanged)

public:
    explicit ParallelCoordinatesPlotDataModel(QObject *parent = nullptr);

    void setData(const QVector<QVector<qreal> > &data,
                 const QStringList &names);
    void setData(const QVector<QVector<qreal> > &data);
    void setNames(const QStringList &names);

    void setBoxplotData(const QVector<QVector<QVector<qreal> > > &boxPlotData);
    void setGoals(const QVariantList &goals);
    void setThresholds(const QVariantList &thresholds);

    bool displayPreferences() const;

    QJsonDocument boxplotDataJson() const;
    QJsonDocument scaleJson() const;

signals:
    void boxplotDataChanged();
    void scaleChanged();
    void displayPreferencesChanged();

public slots:
    /*!
     * \brief Toggle the display of the preference bars. When the bars are not
     *        displayed, the bounds are scaled to the data only.
     * \param True to display. False to hide.
     */
    void setDisplayPreferences(bool disp);

private:
    void generateBoxplotJsonDcoument();
    void generateScaleJsonDcoument();

    QVector<QVector<QVector<qreal> > >  m_boxplotData;
    QJsonDocument m_boxplotDataJson;
    QJsonDocument m_scaleJson;
    bool          m_displayPreferences;
};

} // namespace Visualisation

#endif // PARALLELCOORDINATESPLOTDATAMODEL_H
