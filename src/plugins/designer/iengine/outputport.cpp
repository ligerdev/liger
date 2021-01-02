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
#include <designer/iengine/outputport.h>
#include <designer/iengine/iprocessnode.h>
#include <coreplugin/coreconstants.h>

#include <QtMath>
#include <QDir>
#include <QChar>
#include <QString>
#include <QMessageBox>
#include <QGraphicsScene>

#include <QDebug>

namespace Designer {

OutputPort::OutputPort()
    : m_graphicsItemParent(0),
      m_iProcessNodeParent(0),
      m_portShift(QPointF(4,0)),
      m_portId(-1)
{   
    setPortType(OUTPUT_PORT);
    setPortState(AWAITING_DATA);
}

OutputPort::OutputPort(IProcessNode* parent, const QString &portName)
    : m_graphicsItemParent(0),
      m_iProcessNodeParent(0),
      m_portShift(QPointF(4,0)),
      m_portId(-1)
{
    
    setPortName(portName);
    setPortType(OUTPUT_PORT);
    setPortState(AWAITING_DATA);

    // This sets the iprocess node as the QGraphicsItem parent. This implies that when
    // the iprocess node is deleted, this port will also be deleted.
    m_graphicsItemParent = qgraphicsitem_cast<QGraphicsItem*>(parent);
    if(m_graphicsItemParent != 0)
        setParentItem(m_graphicsItemParent);

    m_iProcessNodeParent = parent;

    //Check if parent has other output ports.
    QList<QGraphicsItem*> children = m_graphicsItemParent->childItems();
    // Count output ports.
    qint32 n_outputs = 0;
    foreach(QGraphicsItem* c, children) {
        if(qgraphicsitem_cast<OutputPort*>(c))
            n_outputs++;
    }

    if(n_outputs > 1) {
        int step = 4*floor(12.0/n_outputs -1);
        int cnt  = 0;
        foreach(QGraphicsItem* ch, children) {
            OutputPort* chi = 0;
            chi = qgraphicsitem_cast<OutputPort*>(ch);
            if(chi) {
                chi->setPortShift(QPointF(4, -8+cnt*step));
                cnt++;
            }
        }
    }
}

OutputPort::OutputPort(IProcessNode* parent, qint32 portId)
    : m_graphicsItemParent(0),
      m_iProcessNodeParent(0),
      m_portShift(QPointF(4,0)),
      m_portId(portId)
{
    setPortType(OUTPUT_PORT);
    setPortState(AWAITING_DATA);

    m_graphicsItemParent = qgraphicsitem_cast<QGraphicsItem*>(parent);
    if(m_graphicsItemParent != 0)
        setParentItem(m_graphicsItemParent);

    m_iProcessNodeParent = parent;

    //Check if parent has other output ports.
    QList<QGraphicsItem*> children = m_graphicsItemParent->childItems();
    // Count output ports.
    qint32 n_outputs = 0;
    foreach(QGraphicsItem* c, children) {
        if(qgraphicsitem_cast<OutputPort*>(c))
            n_outputs++;
    }

    if(n_outputs > 1) {
        int step = 4*floor(12.0/n_outputs -1);
        int cnt  = 0;
        foreach(QGraphicsItem* ch, children) {
            OutputPort* chi = 0;
            chi = qgraphicsitem_cast<OutputPort*>(ch);
            if(chi) {
                chi->setPortShift(QPointF(4, -8+cnt*step));
                cnt++;
            }
        }
    }
}

OutputPort::OutputPort(const OutputPort &oport)
{
    m_portShift = oport.m_portShift;
    m_portId    = oport.m_portId;
    m_graphicsItemParent = oport.m_graphicsItemParent;
    m_iProcessNodeParent = oport.m_iProcessNodeParent;
}

void OutputPort::paint(QPainter *painter,
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

OutputPort::~OutputPort()
{

}

QRectF OutputPort::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath OutputPort::shape() const
{
    QRectF rect = outlineRect();

    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

QVariant OutputPort::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
    if (change == ItemPositionHasChanged && portLink() != 0) {
        portLink()->trackPorts();
    }
    return QGraphicsItem::itemChange(change, value);
}


QRectF OutputPort::outlineRect() const
{
    QRectF rect = QRectF(0,0,Constants::PORT_DEFAULT_SIZE,
                             Constants::PORT_DEFAULT_SIZE);
    rect.adjust(-padding(), -padding(), +padding(), +padding());
    rect.translate(-rect.center());
    QRectF prect= m_graphicsItemParent->boundingRect();
    rect.translate(prect.right()+(Constants::PORT_DEFAULT_SIZE)/2.0,
                   m_portShift.y());
    return rect;
}

QPolygonF OutputPort::outlinePolygon()
{
    return QPolygonF(this->outlineRect());
}

IProcessNode*  OutputPort::iProcessNodeParent() const
{
    return m_iProcessNodeParent.data();
}

QString OutputPort::parentIDStr()
{
    return m_iProcessNodeParent->nodeIDStr();
}

} // namespace Designer
