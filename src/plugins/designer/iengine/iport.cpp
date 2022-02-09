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
#include <designer/iengine/iport.h>
#include <coreplugin/coreconstants.h>

#include <QtWidgets>
#include <QDir>
#include <QChar>
#include <QMessageBox>
#include <QString>

namespace Designer {

IPort::IPort(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_text            ("")
    , m_textColor       (Qt::darkGreen)
    , m_portName        ("")
    , m_backgroundColor (Qt::white)
    , m_outlineColor    (Qt::darkBlue)
    , m_portType        (ABSTRACT_PORT)
    , m_portState       (NO_STATE)
    , m_portLink        (0)
    , m_padding         (0)
{
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

IPort::IPort(const IPort &port)
    : m_text            (port.m_text)
    , m_textColor       (port.m_textColor)
    , m_portName        (port.m_portName)
    , m_backgroundColor (port.m_backgroundColor)
    , m_outlineColor    (port.m_outlineColor)
    , m_portType        (port.m_portType)
    , m_portState       (port.m_portState)
    , m_portLink        (port.m_portLink)
    , m_padding         (port.m_padding)
{
}

IPort::~IPort()
{
    if(m_portLink) {
        m_portLink->detachPorts();
    }
}

void IPort::setText(const QString &text)
{
    prepareGeometryChange();
    m_text = text;
    update();
}

QString IPort::text() const
{
    return m_text;
}

void IPort::setTextColor(const QColor &color)
{
    m_textColor = color;
    update();
}

QColor IPort::textColor() const
{
    return m_textColor;
}

void IPort::setOutlineColor(const QColor &color)
{
    m_outlineColor = color;
    update();
}

QColor IPort::outlineColor() const
{
    return m_outlineColor;
}

void IPort::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

QColor IPort::backgroundColor() const
{
    return m_backgroundColor;
}

void IPort::attachLink(IPortLink* link)
{
    if(m_portLink == 0)
        m_portLink = link;
}

void IPort::detachLink()
{
    if(m_portLink) {
        m_portLink = 0;
    }
}

QRectF IPort::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath IPort::shape() const
{
    QRectF rect = outlineRect();

    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

void IPort::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option,
                  QWidget * /* widget */)
{
    QPen pen(m_outlineColor);
    Q_UNUSED(option);
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(m_backgroundColor);

    QRectF rect = outlineRect();
    painter->drawRoundRect(rect, roundness(rect.width()),
                           roundness(rect.height()));

    painter->setPen(textColor());
    painter->drawText(rect, Qt::AlignCenter, m_text);
}

QVariant IPort::itemChange(GraphicsItemChange change,
                           const QVariant &value)
{
    if (change == ItemPositionHasChanged && m_portLink != 0) {
        m_portLink->trackPorts();
    }
    return QGraphicsItem::itemChange(change, value);
}

void IPort::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
    // Snap the mouse to the center of the port
    QPointF itemPosF = this->mapToScene(this->boundingRect().center());
    QPoint  itemPos  = this->scene()->views().first()->mapFromScene(itemPosF);
    QPoint  itemGlobalPos = this->scene()->views().first()->mapToGlobal(itemPos);
    this->cursor().setPos(itemGlobalPos);

    if(!isConnected()) {
        m_padding = 1;
        prepareGeometryChange();
        this->setSelected(true);
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void IPort::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
    QGraphicsItem::hoverMoveEvent(event);
}

void IPort::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if(m_padding !=0) {
        m_padding = 0;
        prepareGeometryChange();
    }
    this->setSelected(false);
    QGraphicsItem::hoverLeaveEvent(event);
}

int IPort::padding() const
{
    return m_padding;
}

void IPort::setPortType(const PortType &portType)
{
    m_portType = portType;
}

QString IPort::portName() const
{
    return m_portName;
}

void IPort::setPortName(const QString &portName)
{
    m_portName = portName;
}

QRectF IPort::outlineRect() const
{
    const int Padding = 4;
    QFont font = qApp->font();
    QFontMetricsF metrics(font);
    QRectF rect = metrics.boundingRect(m_text);
    rect.adjust(-Padding, -Padding, +Padding, +Padding);
    rect.translate(-rect.center());
    return rect;
}

QPolygonF IPort::outlinePolygon()
{
    return QPolygonF(this->outlineRect());
}

int IPort::roundness(double size) const
{
    const int Diameter = 12;
    return 100 * Diameter / int(size);
}

} // namespace Designer
