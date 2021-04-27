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
#ifndef PORTLINKFACTORY_H
#define PORTLINKFACTORY_H

#include <designer/designer_global.h>
#include <QObject>

namespace Designer {
class OutputPort;
class InputPort;
class IPortLink;

class DESIGNER_EXPORT PortLinkFactory : public QObject
{
    Q_OBJECT

public:
    PortLinkFactory(QObject* parent = 0) : QObject(parent) {}
    ~PortLinkFactory() {}

    IPortLink* createPortLink(OutputPort *fromPort, InputPort *toPort);
};

} // namespace Designer

#endif // PORTLINKFACTORY_H
