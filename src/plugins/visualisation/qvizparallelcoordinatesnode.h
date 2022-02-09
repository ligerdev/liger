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
#ifndef  QVIZPARALLELCOORDINATESNODE_H 
#define  QVIZPARALLELCOORDINATESNODE_H
#include <visualisation/qviznode.h>

#include <QObject>

namespace Visualisation {

class ParallelCoordinatesPlotWidget;

class QvizParallelCoordinatesNode : public QVizNode

{
    Q_OBJECT

public:
    QvizParallelCoordinatesNode();
    ~QvizParallelCoordinatesNode();
    void show();

protected slots:
    void updateRobustness(const QStringList& indicators,
                          const QVector<qreal>& parameters);

private:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void customiseWidget(VisualisationWidget* widget);

    ParallelCoordinatesPlotWidget* widgetObj() const;

    void setData();
    void setBoxPlotData();
    void setupShowPrefsCheckbox(QString checkboxStyle);
    static QVector<qreal> getBoxplotEntry(Tigon::Representation::IElementSPtr elm,
                                          Tigon::OptimizationType type);

    QVector<QVector<Tigon::Representation::IElementSPtr> > m_elemData;
    QCheckBox*   m_toggleDispPreferencesButton;
};

} // namespace Visualisation

#endif //  QVIZPARALLELCOORDINATESNODE_H 
