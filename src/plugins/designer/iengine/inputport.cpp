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
#include <designer/iengine/inputport.h>
#include <designer/iengine/iprocessnode.h>
#include <coreplugin/coreconstants.h>

#include <QDir>
#include <QChar>
#include <QString>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QtMath>

#include <QDebug>

using namespace Designer;

InputPort::InputPort()
    : m_graphicsItemParent(0),
      m_iProcessNodeParent(0),
      m_portShift(QPointF(4,0)),
      m_portId(-1)
{

    setPortType(INPUT_PORT);
    setPortState(DETACHED_PORT);
}

InputPort::InputPort(IProcessNode *parent, const QString portName)
    : m_graphicsItemParent(0),
      m_iProcessNodeParent(0),
      m_portShift(QPointF(4,0)),
      m_portId(-1)
{
    setPortName(portName);
    setPortType(INPUT_PORT);
    setPortState(AWAITING_DATA);

    m_graphicsItemParent = qgraphicsitem_cast<QGraphicsItem*>(parent);
    if(m_graphicsItemParent != 0)
        setParentItem(m_graphicsItemParent);

    m_iProcessNodeParent = parent;

    // Check if parent has other input ports.
    QList<QGraphicsItem*> children = m_graphicsItemParent->childItems();
    // Count input ports.
    qint32 n_inputs = 0;
    foreach(QGraphicsItem* c, children) {
        if(qgraphicsitem_cast<InputPort*>(c))
            n_inputs++;
    }

    if(n_inputs > 1) {
        int step = 4*qFloor(12.0/n_inputs -1);
        int cnt  = 0;
        foreach(QGraphicsItem* ch, children) {
            InputPort* chi = 0;
            chi = qgraphicsitem_cast<InputPort*>(ch);
            if(chi) {
                chi->setPortShift(QPointF(4,-8+cnt*step));
                cnt++;
            }
        }
    }
}

InputPort::InputPort(IProcessNode *parent, qint32 portId)
    : m_graphicsItemParent(0),
      m_iProcessNodeParent(0),
      m_portShift(QPointF(4,0)),
      m_portId(portId)
{
    setPortType(INPUT_PORT);
    setPortState(AWAITING_DATA);

    m_graphicsItemParent = qgraphicsitem_cast<QGraphicsItem*>(parent);
    if(m_graphicsItemParent != 0)
        setParentItem(m_graphicsItemParent);

    m_iProcessNodeParent = parent;

    // Check if parent has other input ports.
    QList<QGraphicsItem*> children = m_graphicsItemParent->childItems();
    // Count input ports.
    qint32 n_inputs = 0;
    foreach(QGraphicsItem* c, children) {
        if(qgraphicsitem_cast<InputPort*>(c))
            n_inputs++;
    }

    if(n_inputs > 1) {
        int step = 4*qFloor(12.0/n_inputs -1);
        int cnt  = 0;
        foreach(QGraphicsItem* ch, children) {
            InputPort* chi = 0;
            chi = qgraphicsitem_cast<InputPort*>(ch);
            if(chi) {
                chi->setPortShift(QPointF(4,-8+cnt*step));
                cnt++;
            }
        }
    }
}

InputPort::InputPort(const InputPort &iport)
{
    m_portShift = iport.m_portShift;
    m_portId    = iport.m_portId;
    m_graphicsItemParent = iport.m_graphicsItemParent;
    m_iProcessNodeParent = iport.m_iProcessNodeParent;
}

InputPort::~InputPort()
{

}

void InputPort::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget * /* widget */)
{
    QPen pen(outlineColor());
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(backgroundColor());

    QRectF rect = outlineRect();
    painter->drawRoundRect(rect, roundness(rect.width()),
                           roundness(rect.height()));

    painter->setPen(textColor());
    painter->drawText(rect, Qt::AlignCenter, text());
}


QRectF InputPort::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath InputPort::shape() const
{
    QRectF rect = outlineRect();

    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

QVariant InputPort::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
    if (change == ItemPositionHasChanged && portLink() != 0) {
        portLink()->trackPorts();
    }
    return QGraphicsItem::itemChange(change, value);
}

QRectF InputPort::outlineRect() const
{
    QRectF rect = QRectF(0,0,Constants::PORT_DEFAULT_SIZE,
                             Constants::PORT_DEFAULT_SIZE);
    rect.adjust(-padding(), -padding(), +padding(), +padding());
    rect.translate(-rect.center());
    QRectF prect= m_graphicsItemParent->boundingRect();
    rect.translate(prect.left()-(Constants::PORT_DEFAULT_SIZE)/2.0,
                   m_portShift.y());
    return rect;
}

QPolygonF InputPort::outlinePolygon()
{
    return QPolygonF(this->outlineRect());
}

QString InputPort::parentIDStr()
{
    return m_iProcessNodeParent->nodeIDStr();
}

IProcessNode*  InputPort::iProcessNodeParent() const
{
    return m_iProcessNodeParent.data();
}
