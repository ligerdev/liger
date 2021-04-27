/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef IPORT_H
#define IPORT_H

#include <designer/iengine/iportlink.h>
#include <designer/designer_global.h>
#include <designer/designerconstants.h>

#include <QGraphicsItem>

namespace Designer {

class DESIGNER_EXPORT IPort : public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem)

public:
    explicit IPort(QGraphicsItem* parent = Q_NULLPTR);
    IPort(const IPort &port);
    virtual ~IPort();

    virtual void attachLink(IPortLink* link);
    virtual void detachLink();
    virtual IPortLink* portLink() { return m_portLink; }

    enum { Type = UserType + 20 };
    int type() const Q_DECL_OVERRIDE { return Type; }
    PortState portState() const { return m_portState; }
    void setPortState(PortState state) { m_portState = state; }
    PortType  portType()  const { return m_portType;  }
    bool isConnected() { return (m_portLink == 0 ? false : true); }

    virtual void resetState() {}

    virtual void setText(const QString &text);
    virtual QString text() const;
    virtual void setTextColor(const QColor &color);
    virtual QColor textColor() const;
    virtual QString portName() const;
    virtual void setPortName(const QString &portName);
    virtual void setOutlineColor(const QColor &color);
    virtual QColor outlineColor() const;
    virtual void setBackgroundColor(const QColor &color);
    virtual QColor backgroundColor() const;
    virtual QPolygonF outlinePolygon();
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const  Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) Q_DECL_OVERRIDE;
    virtual QRectF outlineRect() const;

    void setPortType(const PortType &portType);

    int padding() const;

protected:
    virtual int roundness(double size) const;
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value) Q_DECL_OVERRIDE;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;

private:
    QString    m_text;
    QColor     m_textColor;
    QString    m_portName;
    QColor     m_backgroundColor;
    QColor     m_outlineColor;
    PortType   m_portType;
    PortState  m_portState;
    IPortLink* m_portLink;
    int        m_padding;
};

} // namespace Designer

#endif // IPORT_H
