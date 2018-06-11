/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <designer/iengine/portlink.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iport.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>
#include <libavoid/libavoid.h>

#include <boost/math/constants/constants.hpp>
#include <QGraphicsLineItem>
#include <QPen>
#include <QDebug>

using namespace Designer;
const qreal Pi = boost::math::constants::pi<qreal>();

PortLink::PortLink()
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    // Draw links above the Ports.
    setZValue(10000);
    setColor(Qt::darkRed);
}

PortLink::PortLink(OutputPort *fromPort, InputPort *toPort)
    : m_fromPort(fromPort)
    , m_toPort(toPort)
{
    m_fromPort->attachLink(this);
    m_toPort->attachLink(this);

    setFlags(QGraphicsItem::ItemIsSelectable);
    // Draw links above the Ports.
    setZValue(10000);
    setColor(Qt::darkRed);
    trackPorts();
}

PortLink::PortLink(const PortLink &plink)
{
    m_fromPort  = plink.m_fromPort;
    m_toPort    = plink.m_toPort;
    m_color     = plink.m_color;
    m_arrowHead = plink.m_arrowHead;
}

PortLink::~PortLink()
{
    if(m_fromPort) {
        m_fromPort->detachLink();
        m_fromPort = 0;
    }
    if(m_toPort) {
        m_toPort->detachLink();
        m_toPort = 0;
    }

}

void PortLink::detachPorts()
{
    m_fromPort = 0;
    m_toPort = 0;
}

void PortLink::setColor(const QColor &color)
{
    setPen(QPen(color, 1.0));
}

QColor PortLink::color() const
{
    return pen().color();
}

/* --------------------------------------------------------------
 * Methods for tracking ports
 * --------------------------------------------------------------
 */

/*!
 * \brief PortLink::trackPorts
 * \todo Avoid all objects in the scene
 */
void PortLink::trackPorts()
{
    if(m_fromPort == 0 || m_toPort == 0)
        return;

    QPointF lp1 = m_fromPort->scenePos() + m_fromPort->outlineRect().center();
    QPointF lp2 = m_toPort->scenePos() + m_toPort->outlineRect().center();
    QPainterPath path;

    calculateRoutingPath(path,lp1,lp2);

    setPath(path);
    setZValue(10);
}

void PortLink::trackPortsAvoid(QList<IPortLink *> portLinks)
{
    if(m_fromPort == 0 || m_toPort == 0)
        return;

    QPointF lp1 = m_fromPort->scenePos() + m_fromPort->outlineRect().center();
    QPointF lp2 = m_toPort->scenePos() + m_toPort->outlineRect().center();
    QPainterPath path;

    calculateRoutingPath(path,lp1,lp2,portLinks);

    setPath(path);
    setZValue(10);
}

void PortLink::trackPortsAvoid(QList<qreal> xPos, QList<qreal> yPos)
{
    if(m_fromPort == 0 || m_toPort == 0)
        return;

    QPointF lp1 = m_fromPort->scenePos() + m_fromPort->outlineRect().center();
    QPointF lp2 = m_toPort->scenePos() + m_toPort->outlineRect().center();
    QPainterPath path;

    calculateRoutingPath(path,lp1,lp2,xPos,yPos);

    setPath(path);
    setZValue(10);
}

void PortLink::trackPortsAvoid(QList<IProcessNode*> processNodes)
{
    if(m_fromPort == 0 || m_toPort == 0)
        return;

    QPointF lp1 = m_fromPort->scenePos() + m_fromPort->outlineRect().center();
    QPointF lp2 = m_toPort->scenePos() + m_toPort->outlineRect().center();
    QPainterPath path;

    calculateRoutingPath(path,lp1,lp2,processNodes);

    setPath(path);
    setZValue(10);
}

/* --------------------------------------------------------------
 * Protected methods
 * --------------------------------------------------------------
 */

/*!
 * \brief calculateRoutingPath(QPainterPath& painterPath, QList<IPortLink*> portLinks)
 * This function calculates the routing path with curved corned, avoiding only
 * the source and destination points
 * \param painterPath
 * \param strP
 * \param dstP
 */
void PortLink::calculateRoutingPath(QPainterPath& painterPath, const QPointF& strP, const QPointF& dstP)
{
    qreal xshift = 15;
    qreal curvedAmount = 10;

    Avoid::Router* router = createRouter();

    Avoid::Point srcPt(strP.x(),strP.y());
    Avoid::Point dstPt(dstP.x(),dstP.y());
    Avoid::ConnRef* connRef = new Avoid::ConnRef(router, srcPt, dstPt);

    // Set check points
    setCheckPoints(connRef,srcPt,dstPt,xshift);

    // Set positions of the source and destination objects only
    Avoid::Point srcRecCntr(m_fromPort->scenePos().x(),m_fromPort->scenePos().y());
    Avoid::Point dstRecCntr(m_toPort->scenePos().x(),m_toPort->scenePos().y());
    Avoid::Rectangle srcRec(srcRecCntr,Constants::NODE_DEFAULT_SIZE,Constants::NODE_DEFAULT_SIZE);
    Avoid::Rectangle dstRec(dstRecCntr,Constants::NODE_DEFAULT_SIZE,Constants::NODE_DEFAULT_SIZE);
    // Pointers are needed. Otherwise the shape ref will be deleted at the end of
    // this function and liger will crash.
    Avoid::ShapeRef* srcShapeRef = new Avoid::ShapeRef(router,srcRec);
    Avoid::ShapeRef* dstShapeRef = new Avoid::ShapeRef(router,dstRec);
    Q_UNUSED(srcShapeRef) // suppress warnings
    Q_UNUSED(dstShapeRef) // suppress warnings

    // Process the route
    router->processTransaction();

    // Generate Bezier curve
    Avoid::Polygon curvedRoute = connRef->displayRoute().curvedPolyline(curvedAmount);
    setLinkRoute(curvedRoute);

    // Assign to painter path
    updatePainterPath(painterPath);

    //free memory
    delete router;
}

/*!
 * \brief PortLink::calculateRoutingPath(QPainterPath& painterPath, QList<IPortLink*> portLinks)
 * This function calculates the routing path with curved corned, avoiding all nodes in
 * the QList
 * \param painterPath
 * \param strP
 * \param dstP
 * \param portLinks
 */
void PortLink::calculateRoutingPath(QPainterPath& painterPath,
                                    const QPointF& strP, const QPointF& dstP,
                                    QList<IPortLink*> portLinks)
{
    qreal xshift = 15;
    qreal curvedAmount = 10;

    Avoid::Router* router = createRouter();

    Avoid::Point srcPt(strP.x(),strP.y());
    Avoid::Point dstPt(dstP.x(),dstP.y());
    Avoid::ConnRef* connRef = new Avoid::ConnRef(router, srcPt, dstPt);

    // Set check points
    setCheckPoints(connRef,srcPt,dstPt,xshift);

    // Set positions of all objects (nodes)
    QList<Avoid::ShapeRef*> shapeRefList;
    Avoid::PointList cntrList;
    std::vector<Avoid::Rectangle> recList;
    for(int i=0; i<portLinks.size(); i++){
        cntrList.push_back(Avoid::Point(portLinks[i]->scenePos().x(),
                                        portLinks[i]->scenePos().y()));
        recList.push_back(Avoid::Rectangle(cntrList[i],
                                           Constants::NODE_DEFAULT_SIZE,
                                           Constants::NODE_DEFAULT_SIZE));
        shapeRefList.push_back(new Avoid::ShapeRef(router,recList[i],i));
    }

    // Process the route
    router->processTransaction();

    // Generate Bezier curve
    Avoid::Polygon curvedRoute = connRef->displayRoute()
            .curvedPolyline(curvedAmount);
    setLinkRoute(curvedRoute);

    // Assign to painter path
    updatePainterPath(painterPath);

    //free memory
    delete router;
}

void PortLink::calculateRoutingPath(QPainterPath &painterPath, const QPointF &strP,
                              const QPointF &dstP, QList<qreal> xPos, QList<qreal> yPos)
{
    qreal xshift = 15;
    qreal curvedAmount = 10;

    Avoid::Router* router = createRouter();

    Avoid::Point srcPt(strP.x(),strP.y());
    Avoid::Point dstPt(dstP.x(),dstP.y());
    Avoid::ConnRef* connRef = new Avoid::ConnRef(router, srcPt, dstPt);

    // Set check points
    setCheckPoints(connRef,srcPt,dstPt,xshift);

    // Set positions of all objects (nodes)
    QList<Avoid::ShapeRef*> shapeRefList;
    std::vector<Avoid::Point> cntrList;
    std::vector<Avoid::Rectangle> recList;
    for(int i=0; i<xPos.size(); i++){
        cntrList.push_back(Avoid::Point(xPos.at(i),yPos.at(i)));
        recList.push_back(Avoid::Rectangle(cntrList.at(i),
                                           Constants::NODE_DEFAULT_SIZE,
                                           Constants::NODE_DEFAULT_SIZE));
        shapeRefList.push_back(new Avoid::ShapeRef(router,recList.at(i)));
    }

    // Process the route
    router->processTransaction();

    // Generate Bezier curve
    Avoid::Polygon curvedRoute = connRef->displayRoute().curvedPolyline(curvedAmount);
    setLinkRoute(curvedRoute);

    // Assign to painter path
    updatePainterPath(painterPath);

    //free memory
    delete router;
}

void PortLink::calculateRoutingPath(QPainterPath& painterPath,
                                    const QPointF &strP, const QPointF &dstP,
                                    QList<IProcessNode*> processNodes)
{
    qreal xshift = 15;
    qreal curvedAmount = 10;

    Avoid::Router* router = createRouter();

    Avoid::Point srcPt(strP.x(),strP.y());
    Avoid::Point dstPt(dstP.x(),dstP.y());
    Avoid::ConnRef* connRef = new Avoid::ConnRef(router, srcPt, dstPt);

    // Set check points
    setCheckPoints(connRef,srcPt,dstPt,xshift);

    // Set positions of all objects (nodes)
    QList<Avoid::ShapeRef*> shapeRefList;
    std::vector<Avoid::Point> cntrList;
    std::vector<Avoid::Rectangle> recList;
    for(int i=0; i < processNodes.size(); i++){
        IProcessNode* node = processNodes.at(i);
        cntrList.push_back(Avoid::Point(node->scenePos().x(),node->scenePos().y()));
        recList.push_back(Avoid::Rectangle(cntrList.at(i),
                                           Constants::NODE_DEFAULT_SIZE,
                                           Constants::NODE_DEFAULT_SIZE));
        shapeRefList.push_back(new Avoid::ShapeRef(router,recList.at(i),i));
        qDebug()<< "Portlink::trackPorts(processNodes): Cntr : " << i << " x: "
                << cntrList.at(i).x << " y: " << cntrList.at(i).y;
    }

    qDebug() << "Portlink::trackPorts(processNodes): Number of Nodes found: "
             << processNodes.size();

    // Process the route
    router->processTransaction();

    // Generate Bezier curve
    Avoid::Polygon curvedRoute = connRef->displayRoute().curvedPolyline(curvedAmount);
    setLinkRoute(curvedRoute);

    // Assign to painter path
    updatePainterPath(painterPath);

    //free memory
    delete router;
}

void PortLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                     QWidget *)
{
    if(m_fromPort == 0 || m_toPort == 0)
        return;
    if (m_fromPort->collidesWithItem(m_toPort))
        return;

    QPen myPen = pen();
    myPen.setColor(m_color);
    qreal arrowSize = 7;
    painter->setPen(myPen);

    QPointF lp2 = m_toPort->scenePos() + m_toPort->outlineRect().center();
    //QPointF lp1 = m_fromPort->scenePos() + m_fromPort->outlineRect().center();

    // Arrows
    double angle = Pi;
    QPointF arrowP1 = lp2 + QPointF(sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = lp2 + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    m_arrowHead.clear();
    m_arrowHead << lp2 << arrowP1 << arrowP2;

    // Change the style of the line to dashed line when selected
    if (isSelected()) {
        painter->setPen(QPen(m_color, 1.5, Qt::DashLine));
    }

    // Draw the links between the start and end nodes
    QPainterPath path;
    updatePainterPath(path);
    setPath(path);
    painter->drawPath(path);
    painter->setBrush(m_color);
    painter->drawPolygon(m_arrowHead);
}

/* -----------------------
 * Provate functions
 * -----------------------
 */

Avoid::Router* PortLink::createRouter()
{
    // Routing options
    Avoid::RouterFlag routerFlag = Avoid::OrthogonalRouting;
    qreal shapeBuffDist = 10;
    qreal segmentPenalty = 10;

    // Create new empty router with router flag already set
    Avoid::Router* router = new Avoid::Router(routerFlag);
    router->setRoutingPenalty(Avoid::RoutingParameter::shapeBufferDistance,shapeBuffDist);
    router->setRoutingPenalty(Avoid::RoutingParameter::segmentPenalty,segmentPenalty);

    return router;
}

void PortLink::setCheckPoints(Avoid::ConnRef *connRef, const Avoid::Point srcPt, const Avoid::Point dstPt, int xshift)
{
    Avoid::Point chkPt1(srcPt.x+xshift,srcPt.y);
    Avoid::Point midPt((srcPt.x+dstPt.x)/2, (srcPt.y+dstPt.y)/2);
    Avoid::Point chkPt2(dstPt.x-xshift,dstPt.y);
    Avoid::Checkpoint checkPt1(chkPt1);
    Avoid::Checkpoint checkMidPt(midPt);
    Avoid::Checkpoint checkPt2(chkPt2);
    QVector<Avoid::Checkpoint> checkPts;
    checkPts.push_back(checkPt1);       //
    checkPts.push_back(checkMidPt);     // order matters
    checkPts.push_back(checkPt2);       //
    connRef->setRoutingCheckpoints(checkPts.toStdVector());
}

void PortLink::updatePainterPath(QPainterPath& painterPath)
{
    for(size_t i=0; i<m_linkRoute.size(); i++){
        QPointF point(m_linkRoute.ps[i].x,m_linkRoute.ps[i].y);
        if(i==0){
            painterPath.moveTo(point);
        }else if(m_linkRoute.ts[i]=='C'){
            QPointF c2(m_linkRoute.ps[i+1].x,m_linkRoute.ps[i+1].y);
            QPointF c3(m_linkRoute.ps[i+2].x,m_linkRoute.ps[i+2].y);
            painterPath.cubicTo(point,c2,c3);
            i += 2;
        }else{
            painterPath.lineTo(point);
        }
    }
}

void PortLink::setLinkRoute(Avoid::Polygon route)
{
    m_linkRoute = route;
}
