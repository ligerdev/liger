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
#ifndef VISUALISATIONDATAMODEL_H
#define VISUALISATIONDATAMODEL_H

#include <QStringList>
#include <QVariantList>
#include <QString>
#include <QVector>
#include <QJsonDocument>
#include <QObject>

namespace Visualisation {

class VisualisationDataModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QJsonDocument dataJson MEMBER m_dataJson
               READ dataJson NOTIFY dataChanged)
    Q_PROPERTY(int dataRows READ dataRows NOTIFY dataChanged)
    Q_PROPERTY(QStringList allNames
               READ allNames WRITE setNames NOTIFY dataChanged)
    Q_PROPERTY(QVariantList selectedIndices READ selectedIndices
               WRITE setSelectedIndices NOTIFY selectedIndicesChanged)
    Q_PROPERTY(QVariantList colCategories READ colCategories
               WRITE setColCategories NOTIFY categoriesChanged)
    Q_PROPERTY(QVariantList goals READ goals WRITE setGoals NOTIFY goalsChanged)
    Q_PROPERTY(QVariantList thresholds READ thresholds
               WRITE setThresholds NOTIFY thresholdsChanged)
    Q_PROPERTY(QVariantList preferenceDirerctions READ preferenceDirerctions
               WRITE setPreferenceDirerctions NOTIFY preferenceDirerctionsChanged)

public:
    explicit VisualisationDataModel(QObject *parent = nullptr);

    void virtual setData(const QVector<QVector<qreal> > &data,
                         const QStringList &names);
    void virtual setData(const QVector<QVector<qreal> > &data);
    void virtual setNames(const QStringList &names);


    QVector<QVector<qreal> > rawData() const;
    QStringList allNames() const;
    QJsonDocument dataJson() const;

    int dataRows() const;
    int dataCols() const;

    QVariantList selectedIndices() const;
    void setSelectedIndices(const QVariantList &selectedIndices);

    QVariantList colCategories() const;
    void setColCategories(const QVariantList &colCategories);

    QList<int> brushedIndices() const;
    QMap<int, QVector<qreal> > brushedBounds() const;

    QVariantList goals() const;
    void virtual setGoals(const QVariantList &goals);

    QVariantList thresholds() const;
    void virtual setThresholds(const QVariantList &thresholds);

    QVariantList preferenceDirerctions() const;
    void setPreferenceDirerctions(const QVariantList &preferenceDirerctions);

    /*!
     * \brief Include all the points in the list of brushed indices.
     * \todo  This is required when the model is updated after some points
     *        were already brushed. The javascript does not send the updated
     *        indices that inclued the entire data set. Consider fixing this in
     *        the javascript.
     */
    ///\note Brushed indicies should only be changed from the javascript. That's
    /// why it is called brushed indices. The following should not exist at all.
    void resetBrushedIndices();

signals:
    void dataChanged();
    void namesChanged();
    void selectedIndicesChanged();
    void brushedIndicesChanged(const QList<int>& brushedIndices);
    void brushedBoundsChanged(const int& index, const qreal& lb, const qreal& ub);
    void categoriesChanged();
    void goalsChanged();
    void thresholdsChanged();
    void preferenceDirerctionsChanged();


public slots:
    /*!
     * \brief Set the indices of the brushed points from the javascript.
     * \param The indices of the selected points after brushing
     */
    void setBrushedIndices(const QVariantList &brushedIndices);
    /*!
     * \brief Set the lower and upper bounds of the brushing
     * \param The index of the data column
     * \param The upper bound
     * \param The lower bound
     */
    void setBrushedBounds(const int& column, QVariant lb, QVariant ub);

protected:
    void generateDataJsonDocument();


private:
    QVector<QVector<qreal>> m_data;
    QJsonDocument m_dataJson;
    QStringList   m_names;
    QString       m_dataString;
    QVariantList  m_selectedIndices; // List<int> is not recognised by js
    QList<int>    m_brushedIndices;
    QMap<int, QVector<qreal> >  m_brushedBounds;
    QVariantList  m_colCategories; // category of the columns
    QVariantList  m_goals;
    QVariantList  m_thresholds;
    /*!
     * \brief the directions of the preference (goal and threshold). 1 represents
     * that the clinet is interested in area below the goal/threshold; -1 for above;
     * null for undefined.
     */
    QVariantList  m_preferenceDirerctions;
};

} // namespace Visualisation
#endif // VISUALISATIONDATAMODEL_H
