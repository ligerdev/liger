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
#ifndef IPORTLINK_H
#define IPORTLINK_H

#include <designer/designer_global.h>
#include <QGraphicsPathItem>

namespace Designer {

class IPort;

class DESIGNER_EXPORT IPortLink : public QGraphicsPathItem
{
    Q_INTERFACES(QGraphicsItem)

public:
    explicit IPortLink(QGraphicsItem *parent = Q_NULLPTR) : QGraphicsPathItem(parent) {}
    virtual ~IPortLink() {}

    virtual IPort* fromPort() const = 0;
    virtual IPort* toPort()   const = 0;
    virtual void detachPorts()      = 0;

    virtual void setColor(const QColor& color) = 0;
    virtual QColor color() const = 0;

    virtual void trackPorts() = 0;
};

} // namespace Designer


#endif // IPORTLINK_H
