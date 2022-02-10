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
#ifndef OBJECTIVERANKWIDGET_H
#define OBJECTIVERANKWIDGET_H
#include <visualisation/core/visualisationwidget.h>

#include <QObject>

namespace Visualisation {

class ObjectiveRankWidget : public VisualisationWidget
{
    Q_OBJECT

public:
    explicit ObjectiveRankWidget(QWidget *parent = 0);
    ~ObjectiveRankWidget();
    void initialise();

    void setObjReductionData(
            const QVector<bool>& essentialObjectiveSet,
            const QVector<qreal>& errorPerObjective,
            const QVector<qreal>& variancePerObjective,
            const QVector<qreal>& variancePerPCA,
            const QMap<int, QVector<int>> correlatedObjectivesIdx,
            const QMap<int, QVector<double>> correlatedObjectivesStrength,
            const QStringList& objectiveNames);

    void setVisualisationType(int visualisationType);

signals:
    void runObjectiveReductionAlgorithm();

};

} // namespace Visualisation

#endif // OBJECTIVERANKWIDGET_H
