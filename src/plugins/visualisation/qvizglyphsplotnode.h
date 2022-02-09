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
#ifndef QVIZGLYPHSPLOTNODE_H
#define QVIZGLYPHSPLOTNODE_H
#include <visualisation/visualisation_global.h>
#include <visualisation/visualisationconstants.h>
#include <visualisation/qviznode.h>

#include <QObject>

namespace Visualisation {

class QVizGlyphsPlotNode: public QVizNode
{
    Q_OBJECT

public:
    QVizGlyphsPlotNode();
    ~QVizGlyphsPlotNode();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    void customiseWidget(VisualisationWidget* widget);
};

} // namespace Visualisation

#endif // QVIZGLYPHSPLOTNODE_H
