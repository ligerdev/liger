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
#ifndef OUTPUTPORT_H
#define OUTPUTPORT_H
#include <designer/iengine/iport.h>

#include <QPen>
#include <QStyle>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPointer>

namespace Designer {

class IProcessNode;

class DESIGNER_EXPORT OutputPort : public IPort
{

public:
    enum { Type = UserType + 22 };
    OutputPort();
    explicit OutputPort(IProcessNode* parent,
                        const QString& portName = QString("OutputPort"));
    explicit OutputPort(IProcessNode* parent, qint32 portId);
    // Copy constructor needed by the qt meta object system.
    OutputPort(const OutputPort &oport);
    ~OutputPort();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);
    QGraphicsItem* portParent() const { return m_graphicsItemParent; }
    IProcessNode *iProcessNodeParent() const;

    virtual QPolygonF outlinePolygon();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    int type() const { return Type; }
    QRectF outlineRect() const;

    void setPortShift(QPointF shift) { m_portShift = shift; }
    void setPortId(qint32 portId)    { m_portId = portId; }
    qint32 portId()  { return m_portId; }
    QString parentIDStr();

    void resetState() { setPortState(isConnected() ? AWAITING_DATA : DETACHED_PORT); }

signals:
    void outputPortDataSentSignal(OutputPort* out);

public slots:

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);

private:
    QGraphicsItem*          m_graphicsItemParent;
    QPointer<IProcessNode>  m_iProcessNodeParent;
    QPointF        m_portShift;
    qint32         m_portId;
};

} // namespace Designer

Q_DECLARE_METATYPE(Designer::OutputPort)

#endif // OUTPUTPORT_H
