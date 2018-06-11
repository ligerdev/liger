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
#ifndef NODEIDFACTORY_H
#define NODEIDFACTORY_H

#include <designer/designer_global.h>

#include <QObject>

namespace Designer {
class NodeID;

class DESIGNER_EXPORT NodeIDFactory : public QObject
{
    Q_OBJECT

public:
    static NodeIDFactory* instance();
    NodeIDFactory();
    ~NodeIDFactory();

    NodeID* createNodeID();

};

} // namespace Designer

#endif // NODEIDFACTORY_H
