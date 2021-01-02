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
#ifndef PORTLINK_H
#define PORTLINK_H

#include <designer/iengine/iportlink.h>
#include <libavoid/geomtypes.h>
#include <designer/iengine/iport.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>

#include <QColor>
#include <QPointer>
#include <QObject>

class QColor;

namespace Avoid {
class Router;
class ConnRef;
}

namespace Designer {

class IPort;
class InputPort;
class OutputPort;
class IProcessNode;

class DESIGNER_EXPORT PortLink : public IPortLink
{

public:
    PortLink();
    PortLink(OutputPort* fromPort, InputPort* toPort);
    PortLink(const PortLink &plink);
    ~PortLink();

    IPort* fromPort() const { return m_fromPort; }
    IPort* toPort() const   { return m_toPort;   }
    /*!
     * \brief Set the pointers of the ports to 0
     */
    void detachPorts();

    void setColor(const QColor& color);
    QColor color() const;

    void trackPorts();

    ///------------------------------------------------------------
    /*!
     * \brief trackPortsAvoid Avoid all nodes in the editor.
     * Test functions only
     * \param portLinks
     * \todo This function is not finished.
     * \todo These functions should be merged into trackports()
     */
    void trackPortsAvoid(QList<IPortLink*> portLinks);
    void trackPortsAvoid(QList<qreal> xPos, QList<qreal> yPos);
    void trackPortsAvoid(QList<IProcessNode*> processNodes);
    ///-------------------------------------------------------------

protected:
    void calculateRoutingPath(QPainterPath& painterPath, const QPointF &strP, const QPointF &dstP);
    void calculateRoutingPath(QPainterPath& painterPath,
                              const QPointF &strP, const QPointF &dstP,
                              QList<IPortLink*> portLinks);
    void calculateRoutingPath(QPainterPath &painterPath, const QPointF &strP,
                              const QPointF &dstP, QList<qreal> xPos, QList<qreal> yPos);
    void calculateRoutingPath(QPainterPath& painterPath,
                              const QPointF &strP, const QPointF &dstP,
                              QList<IProcessNode*> processNodes);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

private:
    Avoid::Router* createRouter();
    void setCheckPoints(Avoid::ConnRef* connRef, const Avoid::Point srcPt,
                        const Avoid::Point dstPt, int xshift);
    void setLinkRoute(Avoid::Polygon route);
    /*!
     * \brief updatePainterPath This function updates the painter path using
     * points from m_linkRoute
     */
    void updatePainterPath(QPainterPath& painterPath);

    OutputPort*     m_fromPort;
    InputPort*      m_toPort;
    QColor               m_color;
    QPolygonF            m_arrowHead;
    Avoid::Polygon       m_linkRoute;
};

} // namespace Designer

Q_DECLARE_METATYPE(Designer::PortLink)

#endif // PORTLINK_H
